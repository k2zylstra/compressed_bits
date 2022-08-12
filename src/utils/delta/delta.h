#ifndef delta
#define delta

#include "varint.h"
#include <cmath>

#include <vector>
#include <immintrin.h>

// TODO change this to be dyncamic so it is based off the size of an int
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
    unsigned int comp_bytes_start;
    unsigned int comp_bytes_end;
    unsigned int meta_bytes_start;
    unsigned int meta_bytes_end;
    unsigned int * B_starts;
    unsigned int * B_ends;

    int * delta_starts;
    int * delta_ends;

    // holds the compressed bitpacked b' values
    int * comp_delta_starts;
    int * comp_delta_ends;
    // holds the location o f
    int * meta_delta_starts;
    int * meta_delta_ends;
    int * excep_delta_starts;
    int * excep_delta_ends;
    
    // TODO make these private
    unsigned int bprim_starts;
    unsigned int bprim_ends;

    vector<int> initial_vals_starts;
    vector<int> initial_vals_ends;
    
    // encoding values: 1, 2, 4 corresponding to d1, d2, or d4 encoding
    Delta(unsigned int * Bstarts, unsigned int * Bends, unsigned int Bcount, int encoding);
    ~Delta();

    int compress_varint();
    int compress_s4fastpfor();

    int find_match(int * A);

private:
    unsigned int * B_lens;

    int compute_deltas_d4(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length);
    int compute_deltas_d2(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length);
    int compute_deltas_d1(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length);

    int allocate_comp_space(unsigned int * B_arr, int ** meta_arr, int ** comp_arr, int * mc, int * cc, int * ec, unsigned int bprim);
    int s4_fastpfor(int * delta_arr, int ** meta_arrp, int ** comp_arrp, int mc, int cc, int ec, int delta_c, int bprim);
   
    int determine_bprimes();
};

#endif