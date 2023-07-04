/* Delta Object
*   This takes in a set list of sorted intervals and will:
*   - compute the lengths
*   - calculate the deltas for the starts
*   - calculate the deltas for the ends
*   - return in the deltas in place
*/


#include "delta.h"
#include <vector>
#include <iostream>

#include <immintrin.h>

using namespace std;

#define BLOCK_SIZE 256


Delta::~Delta() {
    free(Delta::delta_starts);
    free(Delta::delta_ends);
    cout << "Compressed starts array size in bytes: " << comp_bytes_start << endl;
    cout << comp_delta_starts[comp_bytes_start] << endl;
    free(Delta::B_lens);
    free(Delta::comp_delta_starts);
    free(Delta::comp_delta_ends);
    free(Delta::meta_delta_ends);
    free(Delta::meta_delta_starts);
}

// encoding is for whether to do d4, d2, or d1 encoding: values are 1, 2, or 4
Delta::Delta(unsigned int * Bstarts, unsigned int * Bends, unsigned int Bcount, int encoding)
{
    Delta::B_starts = Bstarts;
    Delta::B_ends = Bends;

    
    int compute;
    switch (encoding)
    {
        case 1:
            Delta::bcount = Bcount - 1;
            Delta::delta_starts = (int*)malloc(sizeof(int)*(Bcount-1));
            Delta::delta_ends = (int*)malloc(sizeof(int)*(Bcount-1));
            Delta::B_lens = (unsigned int*)malloc(sizeof(int)*(Bcount-1));
            compute = Delta::compute_deltas_d1(Bstarts, Bends, Bcount);
            break;
        case 2:
            Delta::bcount = Bcount - 2;
            Delta::delta_starts = (int*)malloc(sizeof(int)*(Bcount-2));
            Delta::delta_ends = (int*)malloc(sizeof(int)*(Bcount-2));
            Delta::B_lens = (unsigned int*)malloc(sizeof(int)*(Bcount-2));
            compute = Delta::compute_deltas_d2(Bstarts, Bends, Bcount);
            break;
        case 4:
            Delta::bcount = Bcount - 4;
            Delta::delta_starts = (int*)malloc(sizeof(int)*(Bcount-4));
            Delta::delta_ends = (int*)malloc(sizeof(int)*(Bcount-4));
            Delta::B_lens = (unsigned int*)malloc(sizeof(int)*(Bcount-4));
            compute = Delta::compute_deltas_d4(Bstarts, Bends, Bcount);
            break;

        default:
            return;
            cerr << "Encoding not correct Option" << endl;
            break;
    }

    // Set initial values
    for (int i = 0; i < encoding; i++) {
        Delta::initial_vals_starts.push_back(Bstarts[i]);
        Delta::initial_vals_ends.push_back(Bends[i]);
    }

    // make sure that the deltas were computed
    if (compute != 0) {
        return;
    }
    Delta::determine_bprimes();
    Delta::compress_s4fastpfor();
}

// TODO will need block starts array to store starting numbers
//  Bstarts and Bends will need to be padded to be greater than 8 and divisible by 4
//  ^ will only be space efficient if Bends and Bstarts are vectors


int Delta::compute_deltas_d1(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length) {

    cout << "==== d1 length: " << B_length << endl;

    ymmregister y1;
    ymmregister y2;
    ymmregister yr;
    ymmregister mask;
    y1.r4 = _mm_setzero_si128();
    y2.r4 = _mm_setzero_si128();
    yr.r4 = _mm_setzero_si128();

    mask.i4[0] = 0xffffffff;
    mask.i4[1] = 0xffffffff;
    mask.i4[2] = 0xffffffff;
    mask.i4[3] = 0xffffffff;

    unsigned int i;

    // i = 1 to store the first option
    for (i = 0; i <= B_length - 5; i+=4) {
        // TODO what if B_length is not deivisible by 8 and therefore tries to acces outside of this
        // sets the ymm registers and subtracts the values to be stored in the result ymm register
        y1.r4 = _mm_setr_epi32(Bstarts[i], Bstarts[i+1], Bstarts[i+2], Bstarts[i+3]);
        y2.r4 = _mm_setr_epi32(Bstarts[i+1], Bstarts[i+2], Bstarts[i+3], Bstarts[i+4]);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
        for (int j = 0; j < 4; j++) {
            Delta::delta_starts[i+j] = yr.i4[j];
        }
        // does the same thing  as the block above just with the ends of the B intervals
        y1.r4 = _mm_setr_epi32(Bends[i], Bends[i+1], Bends[i+2], Bends[i+3]);
        y2.r4 = _mm_setr_epi32(Bends[i+2], Bends[i+3], Bends[i+4], Bends[i+5]);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
        for (int j = 0; j < 4; j++) {
            Delta::delta_ends[i+j] = yr.i4[j];
        }
/*
        y1.r4 = _mm_maskload_epi32((const int *)Bstarts+(i), mask.r4);
        y2.r4 = _mm_maskload_epi32((const int *)Bstarts+(i+1), mask.r4);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
        _mm_maskstore_epi32(Delta::delta_starts+(i), mask.r4, yr.r4);

        y1.r4 = _mm_maskload_epi32((const int *)Bends+(i), mask.r4);
        y2.r4 = _mm_maskload_epi32((const int *)Bends+(i+1), mask.r4);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
        _mm_maskstore_epi32(Delta::delta_ends+(i), mask.r4, yr.r4);
*/
    }
    for (; i < B_length-1; i++) {
        Delta::delta_starts[i] = B_starts[i+1] - B_starts[i];
        Delta::delta_ends[i] = B_ends[i+1] - B_ends[i];
    }
    cout << "This is the last value for i in d1: " << i << endl;
    
    return 0;
}

int Delta::compute_deltas_d2(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length) {

    ymmregister y1;
    ymmregister y2;
    ymmregister yr;
    y1.r4 = _mm_setzero_si128();
    y2.r4 = _mm_setzero_si128();
    yr.r4 = _mm_setzero_si128();

    unsigned int i;
    // i = 2 to store the first 2 options
    // B_length will break out early and then we can do the rest by hand
    for (i = 0; i <= B_length-6; i+=4) {
        // TODO what if B_length is not deivisible by 8 and therefore tries to acces outside of this
        // sets the ymm registers and subtracts the values to be stored in the result ymm register
        y1.r4 = _mm_setr_epi32(Bstarts[i], Bstarts[i+1], Bstarts[i+2], Bstarts[i+3]);
        y2.r4 = _mm_setr_epi32(Bstarts[i+2], Bstarts[i+3], Bstarts[i+4], Bstarts[i+5]);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
        for (int j = 0; j < 4; j++) {
            Delta::delta_starts[i+j] = yr.i4[j];
        }

        // does the same thing  as the block above just with the ends of the B intervals
        y1.r4 = _mm_setr_epi32(Bends[i], Bends[i+1], Bends[i+2], Bends[i+3]);
        y2.r4 = _mm_setr_epi32(Bends[i+2], Bends[i+3], Bends[i+4], Bends[i+5]);
        yr.r4 = _mm_sub_epi32(y2.r4, y1.r4);
        for (int j = 0; j < 4; j++) {
            Delta::delta_ends[i+j] = yr.i4[j];
        }

    }
    for (;i < B_length - 2; i++) {
        Delta::delta_starts[i] = Bstarts[i+2] - Bstarts[i];
        Delta::delta_ends[i] = Bends[i+2] - Bends[i];
    }
    
    return 0;
}

int Delta::compute_deltas_d4(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length)
{
    // zeros out the registers, should be redundant
    // ymmregister is a union defined in the header file

    ymmregister y1;
    ymmregister y2;
    ymmregister yr;
    y1.r4 = _mm_setzero_si128();
    y2.r4 = _mm_setzero_si128();
    yr.r4 = _mm_setzero_si128();
    unsigned int i;

    // i = 4 to store the first 4 options
    for (i = 0; i <= B_length-8; i+=4) {
        // TODO what if B_length is not deivisible by 8floai and therefore tries to acces outside of this
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
    for (; i < B_length - 4; i++) {
        Delta::delta_starts[i] = Bstarts[i+4] - Bstarts[i];
        Delta::delta_ends[i] = Bends[i+4] - Bends[i];
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
    int largest = 0;
    int largest_bits = 0;
    int bprim_tmp = 0;
    int bprim_hst = 0;
    int bprim_opt = 0;
    unsigned int i;
    int tmp_delt;

    int bnsizes[32];
    for (i = 0; i < 32; i++) {
        bnsizes[i] = 0;
    }

    // determine bprim for starts
    for (i = 0; i < Delta::bcount; i++) {
        if(Delta::delta_starts[i] > largest) {
            largest = Delta::delta_starts[i];
        }

        tmp_delt = Delta::delta_starts[i];
        if (tmp_delt < 1) {
            ++bnsizes[0];
        } else {
            ++bnsizes[(int)ceil(log2(tmp_delt))-1];
        }
    }

    largest_bits = (int)ceil(log2(largest));

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
    for (unsigned int i = 0; i < Delta::bcount; i++) {
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

// pass in B_arr
// pass in bprim
// rest should get filled
int Delta::allocate_comp_space(unsigned int * B_arr,
                               int ** meta_arr,
                               int ** comp_arr,
                               int * mc,
                               int * cc,
                               int * ec,
                               unsigned int bprim) {

    unsigned int i;
    int num_bits;
    int ex_c = 0;
    int base_arr_size;
    int metadata_arr_size;
    vector<int> exceptions;
    int B_arr_tmp;

    // Instead of this could possibly store the bnsizes variable from the determin_bprime method
    //  in order to count the number of sizes for each one. Then a simple addition to find how
    //  many exceptions there are. 
    for (i = 0; i < Delta::bcount; i++) {
        //TODO shouldn't be B_arr, should be using delta_arr at this point
        // so we change the B_arr[i] to a signed int
        B_arr_tmp = B_arr[i];
        if (B_arr[i] == 0) {
            num_bits = 0;
        } else {
            num_bits = ceil(log2((int)B_arr[i]));
        }
        if (num_bits > bprim) {
            ex_c += 1;
            exceptions.push_back(i);
        }
    }

    // allocate base array
    // do not subtract ex_c because exceptions will be included just only bprim bits
    // base_arr_size is number of ints
    double dbas = ((double)Delta::bcount*bprim)/(double)(8*sizeof(int));
    base_arr_size = ceil(dbas);
    *cc = base_arr_size*sizeof(int);
    *comp_arr = (int*)malloc(base_arr_size*sizeof(int));


    // allocated metadata array
    // Format: | b | b' | 1*log2(count execptions)| ... list of index of exceptions ... |
    // may have to chnage count of excpetions chun
    metadata_arr_size = 2 + (int)ceil(((log2(ex_c)/8) + ex_c)/sizeof(int));
    *mc = metadata_arr_size;
    *meta_arr = (int*)malloc(metadata_arr_size*sizeof(int));


    return 0;
}

// TODO here
int Delta::compress_s4fastpfor()
{
    int metadata_arr_size_starts;
    int base_arr_size_starts;
    int ex_c_starts;

    int metadata_arr_size_ends;
    int base_arr_size_ends;
    int ex_c_ends;
    allocate_comp_space(Delta::B_starts,
                        &(Delta::meta_delta_starts), 
                        &(Delta::comp_delta_starts),
                        &metadata_arr_size_starts,
                        &base_arr_size_starts,
                        &ex_c_starts,
                        Delta::bprim_starts);
    allocate_comp_space(Delta::B_ends,
                        &(Delta::meta_delta_ends),
                        &(Delta::comp_delta_ends),
                        &metadata_arr_size_ends,
                        &base_arr_size_ends,
                        &ex_c_ends,
                        Delta::bprim_ends);

    Delta::comp_bytes_start = base_arr_size_starts;
    Delta::comp_bytes_end = base_arr_size_ends;
    Delta::meta_bytes_start = metadata_arr_size_starts;
    Delta::meta_bytes_end = metadata_arr_size_ends;

    // Begin filling data
    s4_fastpfor(Delta::delta_starts,
                &(Delta::meta_delta_starts),
                &(Delta::comp_delta_starts),
                metadata_arr_size_starts,
                base_arr_size_starts,
                ex_c_starts,
                Delta::bcount,
                Delta::bprim_starts);
    s4_fastpfor(Delta::delta_ends,
                &(Delta::meta_delta_ends),
                &(Delta::comp_delta_ends),
                metadata_arr_size_ends,
                base_arr_size_ends,
                ex_c_ends,
                Delta::bcount,
                Delta::bprim_ends);
    return 0;
}

// TODO HERE
// private
// TODO will need to be uncommented, just commented out for testing purposes
// mc: meta count
// cc: compressed array count in bytes
// ec: exceptions count
int Delta::s4_fastpfor(int * delta_arr, int ** meta_arrp, int ** comp_arrp, int mc, int cc, int ec, int delta_c, int bprim)
{
    
    int * comp_arr = *comp_arrp;
    //implemented later
    //int * meta_arr = *meta_arrp;

    int r = bprim;
    int c = 0;
    int space_avail = REGISTER_SIZE;
    unsigned int tmp;
    int comp_arr_i = 0; // compact array index
    //int mask = 0xFFFFFFFF; // I don't think I need this because I am using an | or setting it to be 0 all the way across;

    int sub_number_index = 0;

    // TODO put int check to make sure bprim is less than register size
    // TODO get rid of this line, for testing purposes only
    delta_arr[0] = 0x1;

    // 0 out comp_arr[0], rest should be zerod when comp_arr_i is incrememnted
    comp_arr[0] = 0x0;

    for (int i = 0; i < delta_c;) {
        //take r of delta_arr[i]
        tmp = delta_arr[i];
        tmp <<= REGISTER_SIZE - r - sub_number_index;

        // comp_arr register should already be zeroed out
        // put r of delta_arr[i] in comp_arr[comp_arr_i]
        comp_arr[comp_arr_i] = comp_arr[comp_arr_i] | tmp;
        i++;
        if (r == bprim) {
            c += r;
            space_avail = REGISTER_SIZE - c;
            r = space_avail >= bprim ? bprim: space_avail;
            sub_number_index = 0;
        } else {
            comp_arr_i++;

            //sanity check
            // TODO put in error handler outside of this method to check the return value
            // REGISTER_SIZE/8 should give the amount of bytes per REGISTER
            //  and cc / (# of bytes per register) should be how long it can be
            if (comp_arr_i * (REGISTER_SIZE/8) > cc) {
                cerr << "ERROR: index on the compressed array was larger than allocated" << endl;
                return 1;
            }

            // reset values
            comp_arr[comp_arr_i] = 0x0;
            c = 0;
            space_avail = REGISTER_SIZE;
            sub_number_index += r;
            r = bprim;

            tmp = delta_arr[i];
            tmp <<= (REGISTER_SIZE - bprim + sub_number_index);
            comp_arr[comp_arr_i] = comp_arr[comp_arr_i] | tmp;
            i++;
            c+=bprim-sub_number_index;
            space_avail = REGISTER_SIZE-c;
            sub_number_index = 0;
            r = space_avail >= bprim ? bprim: space_avail;
        }
    }

    return 0;
}

int Delta::find_match(int * A)
{
    return 0;
}