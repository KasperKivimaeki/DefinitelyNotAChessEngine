#ifndef MAGIC_HPP
#define MAGIC_HPP


typedef uint64_t uint64;
typedef uint64_t BB;

BB *init_magics();
BB rrmask(int);
BB bbmask(int);
uint64 index_to_uint64(int index, int bits, uint64 m);
uint64 rmask(int sq);
int count_1s(uint64 b);
uint64 ratt(int sq, uint64 block);

#endif
