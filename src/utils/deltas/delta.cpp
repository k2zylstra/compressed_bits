#include "delta.h"
#include "varint.h"

#include <xmmintrin.h>

int Delta::compute_deltas_d2(unsigned int * Bstarts, unsigned int * Bends, unsigned int B_length)
{
    for (int i = 0; i < B_length; i+=2) {
        //store first value,
        //store second value,
        //compute delta with simd instruction
        u_int64_t x1s, x2s;
        u_int64_t x1e, x2e;
        __m64 m1s, m2s;
        __m64 m1e, m2e;
        x1s = Bstarts[i];
        x1s = x1s << 32;
        x1s = x1s | Bstarts[i+1];
        x2s = Bstarts[i+2];
        x2s = x2s << 32;
        x2s = x2s | Bstarts[i+1];

        _mm_sub_pi32((__m64) x2s, (__m64) x1s);
    }

    return 0;
}

int Delta::compress_varint()
{

}

int Delta::find_match(int * A)
{

}