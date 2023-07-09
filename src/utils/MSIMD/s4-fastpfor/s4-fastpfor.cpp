#include "s4-fastpfor.h"
#include <iostream>

using namespace std;

// cc: compressed array count in bytes
int s4_fastpfor(unsigned int * in_arr, unsigned int * comp_arr, unsigned int cc, unsigned int delta_c, unsigned int bprim)
{
    
    // r is always bprim unless the space available to write into in the register
    //  is less than bprim at which point it will be space_available
    int r = bprim;

    // c is the space that has been used in the compressed array index. When the compressed index
    //  flips this is reset to 0
    int c = 0;

    // space available in the register
    int space_avail = REGISTER_SIZE;

    // the r of an integer that will be added into the compressed array at index comp_arr_i
    unsigned int tmp;

    // index in the compressed array
    unsigned int comp_arr_i = 0; // compact array index

    // This is used for a sanity check. It is a one time calculation, where the comp_arr_i should
    //  always be less than. Otherwise memory leaks will happen or the array will bewritten to out
    //  of bounds
    double max_comp_arr_i = cc/(REGISTER_SIZE/8);

    // If not the whole number is written into the array, this number keeps track of the index at
    //  which the bits still need to be written into the next array
    int sub_number_index = 0;

    // TODO put int check to make sure bprim is less than register size

    // 0 out comp_arr[0], rest should be zerod when comp_arr_i is incrememnted
    comp_arr[0] = 0x0;

    for (int i = 0; i < delta_c;) {
        tmp = in_arr[i];

        // this deletes any bits larger than r
        tmp = (tmp << (REGISTER_SIZE - r)) >> (REGISTER_SIZE - r);

        // if r is less than bprim this deletes the extra bits on the right side of the number.
        // in most cases this does nothing unless r is different than bprim
        tmp = (tmp >> (bprim - r));

        // this gets rid of the extra bits to the left. Will not do anything if r is space_available.
        tmp <<= space_avail - r - sub_number_index;

        // put r of in_arr[i] in comp_arr[comp_arr_i]
        comp_arr[comp_arr_i] = comp_arr[comp_arr_i] | tmp;
        i++;
        if (r == bprim) {
            c += r;
            space_avail = REGISTER_SIZE - c;
            r = space_avail >= bprim ? bprim: space_avail;
            sub_number_index = 0;
        } else {
            comp_arr_i++;
            i--;

            //sanity check
            // TODO put in error handler outside of this method to check the return value
            // REGISTER_SIZE/8 should give the amount of bytes per REGISTER
            //  and cc / (# of bytes per register) should be how long it can be
            if (comp_arr_i > max_comp_arr_i) {
                cerr << "ERROR: index on the compressed array was larger than allocated" << endl;
                return 1;
            }

            // reset values
            comp_arr[comp_arr_i] = 0x0;
            c = 0;
            space_avail = REGISTER_SIZE;
            sub_number_index += r;
            r = bprim;

            tmp = in_arr[i];
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