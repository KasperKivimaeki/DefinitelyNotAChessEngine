#ifndef UTIL_HPP
#define UTIL_HPP

#include "bitboard.hpp"

#define MIN(a, b) ((a) - ((b) < (a))*((a) - (b)))
#define MAX(a, b) ((a) - ((b) > (a))*((a) - (b)))

#define _FILE(sq) (sq % 8)
#define _RANK(sq) (7 - sq / 8)

int log2_64 (uint64_t value);
const char* notation(int sq);
const char* notation(Piece p, int color);

#endif
