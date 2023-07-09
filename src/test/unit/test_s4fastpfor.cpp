#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "s4-fastpfor.h"
#include "mt.h"

TEST(compression_tests, test_compression) {
    unsigned int in_array[] = {0, 2, 5, 7, 12, 15, 1023};
    unsigned int out_correct_array[] = {0x0AF9F800, 0, 0};
    unsigned int out_array[3];
    memset(out_array, 0, sizeof(int) * 3);

    // ASSERT_THAT(out_array, testing::ElementsAre(0, 0, 0));
    s4_fastpfor(in_array, out_array, 12, 7, 3);
    EXPECT_THAT(out_array, testing::ElementsAre(out_correct_array[0], out_correct_array[1], out_correct_array[2]));
}

TEST(compression_tests, test_reg_overflow) {
    unsigned int in_array[] = {2,5,8,7,7,12,18,23,24,29,35,40,41,44,49,52,53,57,59,60,63,64,86};
    unsigned int out_correct_array[] = {0x547f1715, 0x830ca5ce, 0x30000000};
    unsigned int out_array[3];
    memset(out_array, 0, sizeof(int) * 3);

    s4_fastpfor(in_array, out_array, 12, 23, 3);
    EXPECT_THAT(out_array, testing::ElementsAre(out_correct_array[0], out_correct_array[1], out_correct_array[2]));
}

TEST(compression_tests, test_bprim_large) {
    unsigned int in_array[] = {0xa356f2, 0x59d378, 0xa3ce264, 0x2bf36a};
    unsigned int out_correct_array[] = {0x64ade474, 0xde271323, 0xf36a0000};
    unsigned int out_array[3];
    memset(out_array, 0, sizeof(int) * 3);

    s4_fastpfor(in_array, out_array, 12, 4, 19);
}

// need compressed array size finder for this
// TEST(compression_tests, test_time_taken) {
//     unsigned int in_array_c = 209715210;
//     unsigned int in_array[in_array_c];

//     for (int i = 0; i < in_array_c; i++) {
//         in_array[i] = genrand_int32();
//     }

//     //start timer
//     s4_fastpfor(in_array, nullptr, out_array)
// }