#ifndef S4FASTPFOR
#define S4FASTPFOR 

#define REGISTER_SIZE 32


// mc: meta count
// cc: compressed array count in bytes
// ec: exceptions count
int s4_fastpfor(unsigned int  * delta_arr
                ,unsigned int * comp_arr
                ,unsigned int cc
                ,unsigned int delta_c
                ,unsigned int bprim);

#endif