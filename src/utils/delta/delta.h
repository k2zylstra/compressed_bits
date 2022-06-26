#ifndef delta
#define delta

#include "varint.h"
#include <cmath>

#include <vector>
#include <immintrin.h>

#define REGISTER_SIZE 32

using namespace std;


union ymmregister {
    __m128i r4;
    unsigned int i4[4];
};

class Delta {

public:
    int* varint_deltastarts;
    int* varint_ends;
    unsigned int bcount;
    unsigned int * B_starts;
    unsigned int * B_ends;

    int * delta_starts;
    int * delta_ends;
    int * comp_delta_starts;
    int * comp_delta_ends;
    int * meta_delta_starts;
    int * meta_delta_ends;
    
    vector<int> initial_vals;
    
    Delta();
    Delta(unsigned int * Bstarts, unsigned int * Bends, unsigned int Bcount);

    int compute_deltas_d4(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length);
    int compress_varint();
    int compress_s4fastpfor();
    int find_match(int * A);

private:
    unsigned int * B_lens;
    unsigned int bprim_starts;
    unsigned int bprim_ends;
    
    int determine_bprimes();
};

#endif