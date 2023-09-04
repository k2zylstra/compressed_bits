#include <gtest/gtest.h>
#include "MetaArray.h"

TEST(meta_array, test_exception_compression) {
    unsigned int in_array[] = {0xa356f2, 0x59d378, 0xa2ce264, 0x2bf36a};
    // should be 10100 (20 in 5), 1011 (0xb in 4), 101000101 (0x145 in 9), and 101 (5 x 3)

    MetaArray * ma = new MetaArray(in_array, 4);
    EXPECT_EQ(ma->exceptionblocks[22].arr[0], 0xa356f000);
    EXPECT_EQ(ma->exceptionblocks[21].arr[0], 0xb3a6f000);
    EXPECT_EQ(ma->exceptionblocks[26].arr[0], 0xa2ce2640);
    EXPECT_EQ(ma->exceptionblocks[20].arr[0], 0xafcda000);
}

TEST(meta_array, test_exception_overflow) {
    unsigned in_array[] = {0xa356f2, 0xb2fe79, 0xe43f2a, 0xc2f821};

    MetaArray * ma = new MetaArray(in_array, 4);
    EXPECT_EQ(ma->exceptionblocks[22].arr[0], 0xa356f2cb);
    EXPECT_EQ(ma->exceptionblocks[22].arr[1], 0xf9ee43f2);
    EXPECT_EQ(ma->exceptionblocks[22].arr[2], 0xb0be0800);
}