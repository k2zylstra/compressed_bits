#include <stdlib.h>
#include <iostream>
#include "immintrin.h"

using namespace std;

union v {
    __m128i a4;
    int a[4];
} v1, v2, v3;

union large {
    __m512 l;
    float a[16];
}t1, t2, tr;

int main(int argc, char ** argv) {
    
    for (int i = 0; i < 16; i++) {
        t1.a[i] = i+3;
    }
    for (int i=  0; i < 16; i++) {
        t2.a[i] = i+1;
    }

    
    v1.a4 = _mm_setzero_si128();
    v2.a4 = _mm_setzero_si128();
    v3.a4 = _mm_setzero_si128();

    v1.a4 = _mm_setr_epi32(9,8,7,6);
    v2.a4 = _mm_setr_epi32(8,3,2,5);

    v3.a4 = _mm_sub_epi32(v1.a4, v2.a4);

    for (int i = 0; i < 4; i++) {
        cout << v3.a[i] << endl;
    }


    return 0;
}