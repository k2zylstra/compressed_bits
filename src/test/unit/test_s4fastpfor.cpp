#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "s4-fastpfor.h"
#include "mt.h"
#include "timer.h"
#include <iostream>

#define BASEARRSIZE 10000000

class TestCompression : public ::testing::Test {
protected:
    uint32_t in_arr[BASEARRSIZE];

    void SetUp() override {
        for (int i = 0; i < BASEARRSIZE; i++) {
            in_arr[i] = genrand_int32();
        }
    }
};

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

TEST(compression_tests, test_get_num_bits_debruijn) {
    unsigned int in_array[] = {
        0, 1, 2, 4, 8, 16, 32, 64,
        128, 256, 512, 1024, 2048, 4096, 8192, 16384,
        32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304,
        8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648
    };
    int out_correct_array[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

    for (int i = 0; i < 32; i++) {
        EXPECT_EQ(calculate_bits_used_debruijan_32(in_array[i]),out_correct_array[i]);

    }
}

TEST_F(TestCompression, test_time_debruijn) {
    start();
    for (int i = 0; i < BASEARRSIZE; i++) {
        calculate_bits_used_debruijan_32(in_arr[i]);
    }
    stop();
    std::cout << report() << std::endl;
}

TEST_F(TestCompression, test_time_log2) {
    start();
    for (int i = 0; i < BASEARRSIZE; i++) {
        calculate_bits_used_log(in_arr[i]);
    }
    stop();
    std::cout << report() << std::endl;
}

TEST_F(TestCompression, test_time_shift) {
    start();
    for (int i = 0; i < BASEARRSIZE; i++) {
        calculate_bits_used_log(in_arr[i]);
    }
    stop();
    std::cout << report() << std::endl;
}

TEST_F(TestCompression, test_time_taken) {

    float max_time = 0.006 * BASEARRSIZE;
    int bprim = 32;
    uint32_t cc = calculate_compressed_size(bprim);
    std::cout << "CC: " << cc << std::endl;
    uint32_t * out_array = (uint32_t *)malloc(cc);
    start();
    for (int i = 0; i < BASEARRSIZE; i+=256) {
        s4_fastpfor(in_arr + i, out_array, cc, 256, bprim);
    }
    stop();

    int time_taken = report();
    ASSERT_LT(max_time, time_taken);

    std::cout << "time taken in microseconds for ten million number and a bprim of 5: " << time_taken << std::endl;
    free(out_array);
}