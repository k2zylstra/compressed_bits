#ifndef METAARRAY
#define METAARRAY

#include <stdint.h>

// TODO consolidate these ussages
#define REGISTER_SIZE 32
#define uintN_t uint32_t

const uint8_t debruijan_values_32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31
};

class MetaArray {
public:
    MetaArray(uintN_t * barr);
    int get_exception_size(); // in bytes
    int get_meta_size(); // in bytes
    int get_n_exception(int block_n, int index);

private:
    int generate_blocks();
    int calculate_bits_used_debruijan_32(uintN_t number);
    int calculate_bits_used_log(uintN_t number);
    int calculate_bits_used_shift(uintN_t number, int bprim);
    uintN_t * metablock;
    uintN_t (* exceptionblocks)[32];
};

#endif
