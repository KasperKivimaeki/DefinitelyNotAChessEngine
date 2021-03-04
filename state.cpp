#include <iostream>
#include <stdlib.h>
#include "state.hpp"
#include "bitboard.hpp"
#include "util.hpp"

int State::Run() {
    BB moveSquares[64] = {};

    BB color = plies % 2 ? bpieces : wpieces;
    BB them = plies % 2 ? wpieces : bpieces;

    BB hknights = knights & color;
    BB hbishops = bishops & color;
    BB hkings   = kings   & color;
    BB hrooks   = rooks   & color;
    BB hpawns   = pawns   & color;
    BB pieces = wpieces | bpieces;

    Piece pawn = plies % 2 ? BPAWN : WPAWN;

    for (int sq = 0; sq < 64; sq++) {
        BB i = ((BB)1) << sq;
        BB occ = pieces;
        if (!(pieces & i)) continue;

        BB ends = 0;

        Piece p = getPiece(i);
        if (hknights & i)      ends |= b->attacks(sq, KNIGHT, occ);
        if (hbishops & i)      ends |= b->attacks(sq, BISHOP, occ);
        if (hrooks & i)        ends |= b->attacks(sq, ROOK, occ);
        else if (hpawns & i)   ends |= (b->attacks(sq, pawn, occ) & them) | b->moves(sq, pawn, occ);
        else if (hkings & i)   ends |= b->attacks(sq, KING, occ);

        ends &= ~color;

        moveSquares[sq] = ends;
    }

    int n = 0;
    for(int i = 0; i < 64; i++) n += b->popCount(moveSquares[i]);

    next = (State **)malloc(n * sizeof(State *));
    fromS = new int[n];
      toS = new int[n];
    int it = 0;

    for (int src = 0; src < 64; src++) {
        if(!moveSquares[src]) continue;
        for (int dst = 0; dst < 64; dst++) {
            BB dstq = ((BB)1) << dst;
            if(!(moveSquares[src] & dstq)) continue;
            State *sn = new State(*this);
            if(sn->move(((BB)1) << src, ((BB)1) << dst)) {
                delete sn; // Move was illegal
            } else {
                fromS[it] = src;
                toS  [it] = dst;
                next [it] = sn;
                it++;
            }
        }
    }
    next_n = it;

    return n;
}

void State::PrintGame() {
    printf("Plies: %d\n", plies);
    printf("  ABCDEFGH  \n");
    for (int r = 7; r >= 0; r--) {
        printf("%d ", r + 1);
        for (int f = 0; f < 8; f++) {
            char c = '.';
            BB sq = ((BB)1) << ((7 - r)*8 + f);
            switch (getPiece(sq)) {
                case KING:   c = 'K'; break;
                case QUEEN:  c = 'Q'; break;
                case ROOK:   c = 'R'; break;
                case BISHOP: c = 'B'; break;
                case KNIGHT: c = 'N'; break;
                case WPAWN:  c = 'P'; break;
                case BPAWN:  c = 'p'; break;
            }
            if (b->getB(bpieces, f, r)) c = tolower(c);
            printf("%c", c);
        }
        printf(" %d\n", r + 1);
    }
    printf("  ABCDEFGH  \n\n");
}

void State::Change(const char* FEN) {
    wpieces = 0;
    bpieces = 0;
    bishops = 0;
    knights = 0;
    kings   = 0;
    rooks   = 0;
    pawns   = 0;
    int sq = 0;
    while(sq < 64) {
        BB i = 1ull << sq;
        int f = _FILE(sq);
        int r = _RANK(sq);
        char c = *FEN++;
        switch (c) {
            case 'q': bpieces |= i; bishops |= i; rooks |= i; ; break;
            case 'Q': wpieces |= i; bishops |= i; rooks |= i; ; break;
            case 'b': bpieces |= i; bishops |= i;             ; break;
            case 'B': wpieces |= i; bishops |= i;             ; break;
            case 'n': bpieces |= i; knights |= i;             ; break;
            case 'N': wpieces |= i; knights |= i;             ; break;
            case 'p': bpieces |= i; pawns   |= i;             ; break; 
            case 'P': wpieces |= i; pawns   |= i;             ; break;
            case 'r': bpieces |= i; rooks   |= i;             ; break;
            case 'R': wpieces |= i; rooks   |= i;             ; break;
            case 'k': bpieces |= i; kings   |= i;             ; break;
            case 'K': wpieces |= i; kings   |= i;             ; break;
            case '/': sq -= 1; break;
            default: sq += c - 49;
        }
        sq++;
    }
}

Piece State::getPiece(int sq) {
    return getPiece(((BB)1) << sq);
}

Piece State::getPiece(BB sq) {
    if (knights & sq) return KNIGHT;
    else if (rooks & bishops & sq) return QUEEN;
    else if (bishops & sq) return BISHOP;
    else if (rooks & sq)   return ROOK;
    else if (kings & sq)   return KING;
    else if (pawns & wpieces & sq)   return WPAWN;
    else if (pawns & bpieces & sq)   return BPAWN;
    return EMPTY;
}

// Returns true, if move was illegal
bool State::move(BB src, BB dst) {
    Piece p = getPiece(src);
    BB inv_src = ~src;
    BB inv_dst = ~dst;
    if (plies % 2) {
        bpieces = bpieces & inv_src | dst;
        wpieces = wpieces & inv_dst;
    } else {
        wpieces = wpieces & inv_src | dst;
        bpieces = bpieces & inv_dst;
    }
    bishops &= inv_dst;
    rooks &= inv_dst;
    kings &= inv_dst;
    knights &= inv_dst;
    pawns &= inv_dst;
    switch(p) {
        case QUEEN:
            bishops = bishops & inv_src | dst;
            rooks   = rooks   & inv_src | dst;
            break;
        case KING:
            kings   = kings   & inv_src | dst;
            break;
        case BISHOP:
            bishops = bishops & inv_src | dst;
            break;
        case ROOK:
            rooks   = rooks   & inv_src | dst;
            break;
        case KNIGHT:
            knights = knights & inv_src | dst;
            break;
        case WPAWN:
        case BPAWN:
            pawns   = pawns   & inv_src | dst;
            break;
    }
    bool x = isCheck();
    plies += 1;

    return x;
}

bool State::isCheck() {
    BB pieces = bpieces | wpieces;
    BB color = plies % 2 ? bpieces : wpieces;
    BB kpos = kings & color;

    int kbit = log2_64(kpos);

    BB knightChecks = knights & b->attacks(kbit, KNIGHT, pieces);
    BB bishopChecks = bishops & b->attacks(kbit, BISHOP, pieces);
    BB   rookChecks =   rooks & b->attacks(kbit, ROOK,   pieces);
    BB   pawnChecks;
    if (plies % 2)
        pawnChecks =    pawns & b->attacks(kbit, BPAWN,  pieces);
    else
        pawnChecks =    pawns & b->attacks(kbit, WPAWN,  pieces);

    return ~color & (knightChecks | bishopChecks | rookChecks | pawnChecks);
}

int State::colorOf(int sq) {
    return 1 && (wpieces & (((BB)1) << sq));
}

int State::TotalMoves(int d) {
    if (d == 0) return 0;
    if (next_n == -1) Run();
    if (d == 1) return next_n;
    int total = 0;
    for(int i = 1; i < next_n; i++) {
        total += TotalMoves(d - 1);
    }
    return total;
}
