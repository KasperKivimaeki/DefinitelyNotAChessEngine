#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "bitboard.hpp"
#include "magic.hpp"
#include "util.hpp"

#define _FILE(sq) (sq % 8)
#define _RANK(sq) (7 - sq / 8)

void BitBoards::initpopCountOfByte256() {
    popCountOfByte256[0] = 0;
    for (int i = 1; i < 256; i++)
        popCountOfByte256[i] = popCountOfByte256[i / 2] + (i & 1);
}

int BitBoards::popCount (BB x) {
    return popCountOfByte256[ x        & 0xff] +
           popCountOfByte256[(x >>  8) & 0xff] +
           popCountOfByte256[(x >> 16) & 0xff] +
           popCountOfByte256[(x >> 24) & 0xff] +
           popCountOfByte256[(x >> 32) & 0xff] +
           popCountOfByte256[(x >> 40) & 0xff] +
           popCountOfByte256[(x >> 48) & 0xff] +
           popCountOfByte256[ x >> 56];
}

void BitBoards::printbb(const char *s, BB b) {
    std::cout << s << std::endl;
    int i = 64;
    for (int i = 0; i < 64; i++) {
        if ((b >> i) & 1) printf("#");
        else printf(".");
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n");
}

int BitBoards::getB(BB b, int file, int rank) {
    return (b >> ((7 - rank) * 8 + file)) & 1;
}

// File and rank are both in [0, 7]
BB BitBoards::board_at(int file, int rank) {
    // Take bit board at square a1 and then adjust its position
    return ((BB) 1) << ((7 - rank) * 8 + file);
}

void BitBoards::gen_movs_for_pieces() {
    initpopCountOfByte256();
    for (int sq = 0; sq < 64; sq++)
    {
        int f = sq % 8;
        int r = 7 - sq / 8;
        if (r < 0 || r > 6) continue;
        BB smovs = board_at(f, r + 1);
        smovs |= (r == RANK_2ND) ? board_at(f, RANK_4TH) : 0;
        WpawnSlideTable[sq] = smovs;

        BB cmovs = 0;
        cmovs |= (f > 0) ? board_at(f - 1, r + 1) : 0;
        cmovs |= (f < 7) ? board_at(f + 1, r + 1) : 0;
        WpawnCaptureTable[sq] = cmovs;
    }
    for (int sq = 0; sq < 64; sq++)
    {
        int f = sq % 8;
        int r = 7 - sq / 8;
        if (r < 0 || r > 6) continue;
        BB smovs = board_at(f, r - 1);
        smovs |= (r == RANK_7TH) ? board_at(f, RANK_5TH) : 0;
        BpawnSlideTable[sq] = smovs;

        BB cmovs = 0;
        cmovs |= (f > 0) ? board_at(f - 1, r - 1) : 0;
        cmovs |= (f < 7) ? board_at(f + 1, r - 1) : 0;
        BpawnCaptureTable[sq] = cmovs;
    }
    for (int sq = 0; sq < 64; sq++)
    {
        int f = sq % 8;
        int r = 7 - sq / 8;
        BB smovs = 0;
        for(int d = -7; d <= 7; d++) {
            if (!d) continue;
            if (r + d < 0 || r + d > 7) continue;
            if (f + d < 0 || f + d > 7) continue;
            smovs |= board_at(f + d, r + d);
        }
        for(int d = -7; d <= 7; d++)  {
            if (!d) continue;
            if (r - d < 0 || r - d > 7) continue;
            if (f + d < 0 || f + d > 7) continue;
            smovs |= board_at(f + d, r - d);
        }
        BishopSlideTable[sq] = smovs;
        QueenSlideTable[sq] = smovs;
    }
    for (int sq = 0; sq < 64; sq++)
    {
        int f = sq % 8;
        int r = 7 - sq / 8;
        BB smovs = 0;
        for(int h = -2; h <= 2; h++)  {
        for(int w = -2; w <= 2; w++)  {
            if (r + h < 0 || r + h > 7) continue;
            if (f + w < 0 || f + w > 7) continue;
            if (h*h + w*w != 5) continue;
            smovs |= board_at(f + w, r + h);
        }
        }
        KnightSlideTable[sq] = smovs;
    }
    for (int sq = 0; sq < 64; sq++)
    {
        int f = sq % 8;
        int r = 7 - sq / 8;
        BB smovs = 0;
        for(int d = -7; d <= 7; d++) {
            if (!d) continue;
            if (r + d >= 0 && r + d < 8) smovs |= board_at(f, r + d);
            if (f + d >= 0 && f + d < 8) smovs |= board_at(f + d, r);
        }
        RookSlideTable[sq] = smovs;
        QueenSlideTable[sq] |= smovs;
    }
    for (int sq = 0; sq < 64; sq++)
    {
        int f = _FILE(sq);
        int r = _RANK(sq);
        BB smovs = 0;
        for(int h = -1; h <= 1; h++)  {
        for(int w = -1; w <= 1; w++)  {
            if (r + h < 0 || r + h > 7) continue;
            if (f + w < 0 || f + w > 7) continue;
            if (h*h + w*w == 0) continue;
            smovs |= board_at(f + w, r + h);
        }
        }
        KingSlideTable[sq] = smovs;
    }
}

void BitBoards::generate_move_tables() {
    gen_movs_for_pieces();
    size_t meml = sizeof(BB) * 64;
    // King
    memcpy(&SlidingTable[KING],   (void *)KingSlideTable, meml);
    memcpy(&CaptureTable[KING],   (void *)KingSlideTable, meml);
    // Queen
    memcpy(&SlidingTable[QUEEN],  (void *)QueenSlideTable, meml);
    memcpy(&CaptureTable[QUEEN],  (void *)QueenSlideTable, meml);
    // Rook
    memcpy(&SlidingTable[ROOK],   (void *)RookSlideTable, meml);
    memcpy(&CaptureTable[ROOK],   (void *)RookSlideTable, meml);
    // Bishop
    memcpy(&SlidingTable[BISHOP], (void *)BishopSlideTable, meml);
    memcpy(&CaptureTable[BISHOP], (void *)BishopSlideTable, meml);
    // Knight
    memcpy(&SlidingTable[KNIGHT], (void *)KnightSlideTable, meml);
    memcpy(&CaptureTable[KNIGHT], (void *)KnightSlideTable, meml);
    // White pawn
    memcpy(&SlidingTable[WPAWN],  (void *)WpawnSlideTable, meml);
    memcpy(&CaptureTable[WPAWN],  (void *)WpawnCaptureTable, meml);
    // Black pawn
    memcpy(&SlidingTable[BPAWN],  (void *)BpawnSlideTable, meml);
    memcpy(&CaptureTable[BPAWN],  (void *)BpawnCaptureTable, meml);
}

void BitBoards::genAttTables() {
    for (int sq = 0; sq < 64; sq++) {
        BB magic = rmagics[sq];
        int r = 7 - sq / 8;
        int f = sq % 8;
        rookAtt[sq] = new BB[1 << rsn[sq]];
        BB *attacks = rookAtt[sq];
        memset(attacks, 0, (1 << rsn[sq]) * sizeof(BB));

        BB edges = 0xff818181818181ff;
        if (r == 7) edges &= 0xffffffffffffff81;
        else if (r == 0) edges &= 0x81ffffffffffffff;
        if (f == 0) edges &= 0xfffefefefefefeff;
        else if (f == 7) edges &= 0xff7f7f7f7f7f7fff;

        BB mask = SlidingTable[ROOK][sq] & ~edges;

        int i = 0;
        // Iterate all submasks
        BB smask = 0;
        do {
            BB key = (magic * smask) >> (64 - rsn[sq]);
            if(attacks[key]) exit(2);
            attacks[key] = slowRookSolve(sq, smask);
            i++;
            smask = (smask - mask) & mask;
        } while(smask);

        if (i != 1 << rsn[sq]) {
            printf("Found %d separate submasks with square %d\n", i, sq);
            exit(1);
        }
    }

    #pragma omp parallel for
    for (int sq = 0; sq < 64; sq++) {
        BB magic = bmagics[sq];
        int r = 7 - sq / 8;
        int f = sq % 8;
        int per = 1 << bsn[sq];
        bishAtt[sq] = new BB[per];
        BB *attacks = bishAtt[sq];
        memset(attacks, 0, per * sizeof(BB));

        BB edges = 0xff818181818181ff;
        BB mask = SlidingTable[BISHOP][sq] & ~edges;

        int i = 0;
        // Iterate all submasks
        BB smask = 0;
        do {
            BB key = (magic * smask) >> (64 - bsn[sq]);
            if(attacks[key]) exit(2);
            attacks[key] = slowBishSolve(sq, smask);
            i++;
            smask = (smask - mask) & mask;
        } while(smask);

        if (i != per) {
            printf("Found %d separate submasks with square %d\n", i, sq);
            exit(1);
        }
    }
}

BB BitBoards::attacks(int sq, Piece p, BB occ) {
    if (p == ROOK) 
        return rookAtt[sq][((occ & rrmask(sq))*rmagics[sq]) >> (64 - rsn[sq])];
    else if (p == BISHOP) 
        return bishAtt[sq][((occ & bbmask(sq))*bmagics[sq]) >> (64 - bsn[sq])];
    else 
        return CaptureTable[p][sq];
}

BB BitBoards::moves(int sq, Piece p, BB occ) {
    if (p == WPAWN) 
        return rookAtt[sq][((occ & SlidingTable[p][sq])*rmagics[sq]) >> (64 - rsn[sq])] & SlidingTable[p][sq] & ~occ;
    else if (p == BPAWN) 
        return rookAtt[sq][((occ & SlidingTable[p][sq])*rmagics[sq]) >> (64 - rsn[sq])] & SlidingTable[p][sq] & ~occ;
    else 
        return attacks(sq, p, occ);
}

BB BitBoards::slowRookSolve(int sq, BB mask) {
    int r = 7 - sq / 8;
    int f = sq % 8;

    BB sol = 0;

    int e1 = 7 - r;
    int e2 = r;
    int e3 = 7 - f;
    int e4 = f;

    for (int u = 1; u <= e1 && !getB(mask, f, r + u - 1); u++) sol |= board_at(f, r + u);
    for (int u = 1; u <= e2 && !getB(mask, f, r - u + 1); u++) sol |= board_at(f, r - u);
    for (int u = 1; u <= e3 && !getB(mask, f + u - 1, r); u++) sol |= board_at(f + u, r);
    for (int u = 1; u <= e4 && !getB(mask, f - u + 1, r); u++) sol |= board_at(f - u, r);

    return sol;
}

BB BitBoards::slowBishSolve(int sq, BB mask) {
    int r = 7 - sq / 8;
    int f = sq % 8;

    BB sol = 0;

    int e1 = MIN(7 - f, 7 - r);
    int e2 = MIN(7 - f, r);
    int e3 = MIN(f, 7 - r);
    int e4 = MIN(f, r);

    for (int u = 1; u <= e1 && !getB(mask, f + u - 1, r + u - 1); u++) sol |= board_at(f + u, r + u);
    for (int u = 1; u <= e2 && !getB(mask, f + u - 1, r - u + 1); u++) sol |= board_at(f + u, r - u);
    for (int u = 1; u <= e3 && !getB(mask, f - u + 1, r + u - 1); u++) sol |= board_at(f - u, r + u);
    for (int u = 1; u <= e4 && !getB(mask, f - u + 1, r - u + 1); u++) sol |= board_at(f - u, r - u);

    return sol;
}
