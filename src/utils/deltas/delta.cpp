#include "delta.h"
#include "varint.h"
#include <vector>

#include <immintrin.h>


int Delta::Delta(unsigned int * Bstarts, unsigned int * Bends, unsigned int Bcount)
{
    Delta::B_starts = Bstarts;
    Delta::B_ends = Bends;
    Delta::bcount = Bcount;

    Delta::delta_starts = malloc(sizeof(int)*Bcount);
    Delta::delta_ends = malloc(sizeof(int)*Bcount):
    Delta::B_lens = malloc(sizeof(int)*Bcount);
    
    Delta::calculate_interval_lengths();
    Delta::determine_bprime();
}

int Delta::compute_deltas_d2(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length)
{
    // zeros out the registers, should be redundant
    // ymmregister is a union defined in the header file
    ymmregister y1 = _mm_setzero_si128();
    ymmregister y2 = _mm_setzero_si128();
    ymmregister yr = _mm_setzero_si128();

    for (int i = 0; i < B_length; i+=4) {

        // sets the ymm registers and subtracts the values to be stored in the result ymm register
        y1.r4 = _mm_setr_epi32(Bstarts[i], Bstarts[i+1], Bstarts[i+2], Bstarts[i+3]);
        y2.r4 = _mm_setr_epi32(Bstarts[i+4], Bstarts[i+5], Bstarts[i+6], Bstarts[i+7]);
        yr.r4 = _mm_sub_epi32(y2, y1);
        for (int j = 0; j < 4; j++) {
            Delta::delta_starts[i+j] = yr.i4[j];
        }

        // does the same thing  as the block above just with the ends of the B intervals
        y1.r4 = _mm_setr_epi32(Bends[i], Bends[i+1], Bends[i+2], Bends[i+3]);
        y2.r4 = _mm_setr_epi32(Bends[i+4], Bends[i+5], Bends[i+6], Bends[i+7]);
        yr.r4 = _mm_sub_epi32(y2, y1);
        for (int j = 0; j < 4; j++) {
            Delta::delta_ends[i+j] = yr.i4[j];
        }

    }

    return 0;
}


int Delta::determine_bprimes()
{
    // b' = min(128*b'+c(b')(b-b'+8))
    // find largest B value
    // in for loop counting down run through example and count examples in b_values
    // keep one number history 
    // as soon as number stops improving set bprim as history number
    unsigned int largest = 0;
    int largest_bits = 0;
    int bprim_tmp = 0;
    int bprim_hst = 0;
    int bprim_opt = 0;
    int i;

    int bnsizes[32];

    // determine bprim for starts
    for (i = 0; i < Delta::bcount; i++) {
        if(Delta::delta_starts[i] > largest) {
            largest = Delta::delta_starts[i];
        }
        ++bnsizes[ceil(log2(Delta::delta_starts[i]))]
    }

    largest_bits = log2(largest);

    bprim_hst = 128*i+bnsizes[largest_bits-1]*(largest_bits-(largest_bits-1)+8);
    for (i = largest_bits-2; i > 0; i--) {
        bprim_tmp = 128*i+bnsizes[i]*(largest_bits-i+8);
        if (bprim_tmp < bprim_hst) {
            bprim_opt = i;
            break;
        }
        bprim_hst = bprim_tmp;
    }

    Delta::bprim_starts = bprim_opt;

    // reset bnsizes
    for (i = 0; i < 32; i++) {
        bnsizes[i] = 0;
    }

    // determine b prime for ends
    for (int i = 0; i < Delta::bcount; i++) {
        if(Delta::delta_ends[i] > largest) {
            largest = Delta::delta_ends[i];
        }
        ++bnsizes[ceil(log2(Delta)::delta_starts[i])]
    }

    largest_bits = log2(largest);

    bprim_hst = 128 * i+bnsizes[largest_bits-1]*(largest_bits=(largest_bits-1)+9);
    for (i = largest_bits-2; i> 0; i--) {
        bprim_tmp = 128 *i + bnsizes[i]*(largets_bits-i+8);
        if (bprim_tmp < bprim_hst) {
            bprim_opt = i;
            break;
        }
        bprim_hst = bprim_tmp;
    }

    Delta::bprim_ends = bprim_opt;

    return 0;
}

// this is for testing another compression scheme; for later implementation
int Delta::compress_varint()
{
    
}

// TODO here
int Delta::compress_s4fastpfor()
{
    //TODO need to compress using two different b primes

    int i;
    int num_bits;
    int ex_c = 0;
    int base_arr_size;
    int metadata_arr_size;
    vector<int> exceptions;

    // Instead of this could possibly store the bnsizes variable from the determin_bprime method
    //  in order to count the number of sizes for each one. Then a simple addition to find how
    //  many exceptions there are. 
    for (i = 0; i < Delta::bcount; i++) {
        num_bits = ceil(log2(Delta::B_starts[i]));
        if (num_bits > Delta::bprim_starts) {
            ex_c += 1;
            exceptions.append(i);
        }
    }

    // allocate base array
    base_arr_size = ceil(((Delta::bcount - ex_c) * Delta::bprim_starts)/8);
    Delta::comp_delta_starts = malloc(base_arr_size);


    // allocated metadata array
    // Format: | b | b' | 1*log2(count execptions)| ... list of index of exceptions ... |
    // may have to chnage count of excpetions chun
    metadata_arr_size = 2 + ceil((log2(ex_c)/8)) + ex_c;
    Delta::meta_delta_starts = malloc(metadata_arr_size);

}

int Delta::find_match(int * A)
{

}