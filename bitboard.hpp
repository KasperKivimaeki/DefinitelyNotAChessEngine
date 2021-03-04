#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "magic.hpp"

#ifndef BITBOARD_HPP
#define BITBOARD_HPP

enum PIECE {
    EMPTY,
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    BPAWN,
    WPAWN,
    PAWN
};

#define Piece PIECE

#define RANK_1ST 0
#define RANK_2ND 1
#define RANK_3RD 2
#define RANK_4TH 3
#define RANK_5TH 4
#define RANK_6TH 5
#define RANK_7TH 6
#define RANK_8TH 7

const int rsn[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12, 
};

const int bsn[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6,
};

typedef uint64_t BB;

class BitBoards {
public:
    BitBoards() {
        generate_move_tables();
        BB *magics = init_magics();
        memcpy(rmagics, magics, 64 * sizeof(BB));
        memcpy(bmagics, &magics[64], 64 * sizeof(BB));
        delete[] magics;
        genAttTables();
    }
    ~BitBoards() {
        for(int i = 0; i < 64; i++) {
            delete[] rookAtt[i];
            delete[] bishAtt[i];
        }
    }

    void initpopCountOfByte256();
    int popCount (BB x);
    void printbb(const char *s, BB b);
    int getB(BB b, int file, int rank);
    BB board_at(int file, int rank);
    void gen_movs_for_pieces();
    void generate_move_tables();

    BB SlidingTable[8][64];

    BB attacks(int sq, Piece p, BB occ);
    void genAttTables();
private:
    unsigned char popCountOfByte256[256];
    BB slowRookSolve(int square, BB mask);
    BB slowBishSolve(int square, BB mask);
    BB *rookAtt[64];
    BB *bishAtt[64];

    BB  BpawnSlideTable[64];
    BB  WpawnSlideTable[64];
    BB  QueenSlideTable[64];
    BB   KingSlideTable[64];
    BB   RookSlideTable[64];
    BB BishopSlideTable[64];
    BB KnightSlideTable[64];

    BB BpawnCaptureTable[64];
    BB WpawnCaptureTable[64];

    BB rmagics[64];
    BB bmagics[64];

    BB CaptureTable[8][64];
};

#endif
