#ifndef MSIMD
#define MSIMD

#include "delta/delta.h"
#include "MetaArray/MetaArray.h"
#include <stdint.h>

#define BLOCK_SIZE 256


class ModSimd {
public:
    uint32_t ** start_values;

    ModSimd(uint32_t * B_starts, uint32_t * B_ends, uint32_t b_count);
    ~ModSimd();

    // returns 0 for no overlaps, and 1 for overlaps
    int exists_overlaps(unsigned int * A_starts, unsigned int * A_ends);

    // returns count of overlaps and -1 for error
    int set_overlap_count();

    // returns count of overlaps and -1 for error
    int interval_overlap_count(unsigned int A_start, unsigned int A_end);

    // returns 0 for success and -1 for error. Output is stored in overlapping_intervals array
    int enumerate_overlaps(unsigned int * A_starts, unsigned int * A_ends, unsigned int * overlapping_intervals);
    
private: 

    // one dl, ds, and start_values per compression block
    Delta ** dl;
    Delta ** ds;

    MetaArray ** md;

    // should be indexed at -1 of the desired packed array size
    uint8_t * ea[32];
    uint16_t block_size = BLOCK_SIZE;
};

#endif