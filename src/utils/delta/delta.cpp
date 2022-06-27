#include "delta.h"
#include <vector>
#include <iostream>

#include <immintrin.h>

using namespace std;

#define BLOCK_SIZE 256


Delta::Delta() {
}

Delta::Delta(unsigned int * Bstarts, unsigned int * Bends, unsigned int Bcount)
{
    Delta::B_starts = Bstarts;
    Delta::B_ends = Bends;
    Delta::bcount = Bcount;


    Delta::delta_starts = (int*)malloc(sizeof(int)*Bcount);
    Delta::delta_ends = (int*)malloc(sizeof(int)*Bcount);
    Delta::B_lens = (unsigned int*)malloc(sizeof(int)*Bcount);
    
    int compute = Delta::compute_deltas_d4(Bstarts, Bends, Bcount);
    if (compute != 0) {
        return;
    }
    Delta::determine_bprimes();
}

// TODO will need block starts array to store starting numbers
//  Bstarts and Bends will need to be padded to be greater than 8 and divisible by 4
//  ^ will only be space efficient if Bends and Bstarts are vectors


// This is D4 compression
int Delta::compute_deltas_d4(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length)
{
    // zeros out the registers, should be redundant
    // ymmregister is a union defined in the header file

    if (B_length % 4 != 0 || B_length < 8) {
        //This method will error out if otherwise
        return 1;
    }

    ymmregister y1;
    ymmregister y2;
    ymmregister yr;
    y1.r4 = _mm_setzero_si128();
    y2.r4 = _mm_setzero_si128();
    yr.r4 = _mm_setzero_si128();

    for (int i = 0; i < B_length-4; i+=4) {
        // TODO what if B_length is not deivisible by 8 and therefore tries to acces outside of this
        // sets the ymm registers and subtracts the values to be stored in the result ymm register
        y1.r4 = _mm_setr_epi32(Bstarts[i], Bstarts[i+1], Bstarts[i+2], Bstarts[i+3]);
        y2.r4 = _mm_setr_epi32(Bstarts[i+4], Bstarts[i+5], Bstarts[i+6], Bstarts[i+7]);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
        for (int j = 0; j < 4; j++) {
            Delta::delta_starts[i+j] = yr.i4[j];
        }

        // does the same thing  as the block above just with the ends of the B intervals
        y1.r4 = _mm_setr_epi32(Bends[i], Bends[i+1], Bends[i+2], Bends[i+3]);
        y2.r4 = _mm_setr_epi32(Bends[i+4], Bends[i+5], Bends[i+6], Bends[i+7]);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
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
    int tmp_delt;

    int bnsizes[32];

    // determine bprim for starts
    for (i = 0; i < Delta::bcount; i++) {
        if(Delta::delta_starts[i] > largest) {
            largest = Delta::delta_starts[i];
        }

        tmp_delt = Delta::delta_starts[i];
        if (tmp_delt < 1) {
            ++bnsizes[0];
        } else {
            ++bnsizes[(int)ceil(log2(tmp_delt))];
        }
    }

    largest_bits = log2(largest);

    bprim_hst = BLOCK_SIZE * i + bnsizes[largest_bits-1] * (largest_bits-(largest_bits-1)+8);
    for (i = largest_bits-2; i > 0; i--) {
        bprim_tmp = BLOCK_SIZE*i+bnsizes[i]*(largest_bits-i+8);
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
    largest = 0;

    // determine b prime for ends
    for (int i = 0; i < Delta::bcount; i++) {
        if(Delta::delta_ends[i] > largest) {
            largest = Delta::delta_ends[i];
        }

        tmp_delt = Delta::delta_ends[i];
        if (tmp_delt < 1) {
            ++bnsizes[0];
        } else {
            ++bnsizes[(int)ceil(log2(tmp_delt))];
        }
    }

    largest_bits = log2(largest);

    bprim_hst = BLOCK_SIZE* i+bnsizes[largest_bits-1]*(largest_bits-(largest_bits-1)+8);
    for (i = largest_bits-2; i> 0; i--) {
        bprim_tmp = BLOCK_SIZE*i + bnsizes[i]*(largest_bits-i+8);
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
    return 0;   
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
        num_bits = (int)ceil(log2(Delta::B_starts[i]));
        if (num_bits > Delta::bprim_starts) {
            ex_c += 1;
            exceptions.push_back(i);
        }
    }

    // allocate base array
    base_arr_size = (int)ceil(((Delta::bcount - ex_c) * Delta::bprim_starts)/8);
    Delta::comp_delta_starts = (int*)malloc(base_arr_size);


    // allocated metadata array
    // Format: | b | b' | 1*log2(count execptions)| ... list of index of exceptions ... |
    // may have to chnage count of excpetions chun
    metadata_arr_size = 2 + (int)ceil((log2(ex_c)/8)) + ex_c;
    Delta::meta_delta_starts = (int*)malloc(metadata_arr_size);


    // Begin filling data
    //s4_fastpfor(Delta::meta_delta_starts);

    return 0;
}

// private
// TODO will need to be uncommented, just commented out for testing purposes
/*
int Delta::s4_fastpfor(int * meta_arr, int * comp_arr, int * excep_arr, int mc, int cc, int ec, int bprim)
{
    int c, space_avail, r = REGISTER_SIZE;
    int tmp;
    int * delta; // TODO make sure these are reassigned to the correct memory pointer
    int delta_c;
    int comp_arr_i = 0; // compact array index
    int mask = 0xFFFFFFFF;
    int bits_left;

    int sub_number_index = 0;
    int sub_mask = 0;

    // TODO this is a mess rn, need to run an example through line by line and fix it
    for (int i = 0; i < delta_c;) {
        if (r < bprim) {
            tmp = delta[i];
            tmp <<= (REGISTER_SIZE - bprim - c)
            if (sub_number_index != 0) {
                // need to make tmp smaller because some of it has already been stored
                sub_mask = (2**(bprim-sub_number_index+1))-1;
                tmp &= sub_mask;
            }

            // This line zeros out the comp arr that is going to get a new number in it
            comp_arr[comp_arr_i] = comp_arr[comp_arr_i] & (mask << (REGISTER_SIZE-c));
            comp_arr[comp_arr_i] = comp_arr[comp_arr_i] | tmp;
                
            if (REGISTER_SIZE - bprim -c <= bprim) {
                // full number was written
                ++comp_arr_i;
            }
            c = bprim - r;
            if (r > bprim) {
                sub_number_index = 0;
            } else {
                // This I think is wrong
                sub_number_index = r;
            }

            // TODO I think this is also wrong
            if (bprim - sub_number_index >= r) {
                ++delta_c;
            }
        }
    }
    return 0;
}
*/

int Delta::find_match(int * A)
{
    return 0;
}