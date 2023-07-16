#ifndef S4FASTPFOR
#define S4FASTPFOR 

#define BLOCKSIZE 256

// TODO consolidate these ussages
#define REGISTER_SIZE 32
#define uintN_t uint32_t


#include <stdint.h>

// const uint8_t debruijan_values_32[32] = {
//      0,  9,  1, 10, 13, 21,  2, 29,
//     11, 14, 16, 18, 22, 25,  3, 30,
//      8, 12, 20, 28, 15, 17, 24,  7,
//     19, 27, 23,  6, 26,  5,  4, 31
// };

// const uint8_t debruijan_values_32[32] = {
//     0, 1, 3, 7, 15, 31, 30, 28,
//     24, 17, 2, 4, 9, 18, 5, 10,
//     21, 11, 22, 12, 25, 19, 6, 13,
//     27, 23, 14, 29, 26, 20, 8, 16
// };

// const uint8_t debruijan_values_32[32] = {
//     0, 1, 7, 28, 10, 16, 8, 26,
//     27, 11, 3, 15, 17, 22, 31, 2,
//     19, 4, 6, 9, 23, 12, 30, 18,
//     29, 13, 5, 20, 14, 25, 24, 21
// };
const uint8_t debruijan_values_32[32] = {
    0, 1, 10, 2, 11, 14, 22, 3,
    30, 12, 15, 17, 19, 23, 26, 4,
    31, 9, 13, 21, 29, 16, 18, 25,
    8, 20, 28, 24, 7, 27, 6, 5
};

int calculate_bits_used_debruijan_32(uint32_t number);
int calculate_bits_used_log(uint32_t number);
int calculate_bits_used_shift(uint32_t number);

int calculate_compressed_size(int bprim);
// // allocates the space needed for the storage of the compressed blocks
// int allocate_block_space(unsigned int * B_arr
//                       ,int ** meta_arr
//                       ,int ** comp_arr
//                       ,int * mc
//                       ,int * ec
//                       ,unsigned int bprim
//                       );

// returns integer representing number of bytes allocated. And allocates
//  memory for the block_p of size int returned
// count is number of elements to be compressed (should most likely be block size)
// bprim is the size of each element
// block_p is location of allocated space
int allocate_block_space(uint32_t count, uint32_t bprim, uint32_t * block_p);

// mc: meta count
// cc: compressed array count in bytes
// ec: exceptions count
int s4_fastpfor(unsigned int  * delta_arr
                ,unsigned int * comp_arr
                ,unsigned int cc
                ,unsigned int delta_c
                ,unsigned int bprim);

#endif