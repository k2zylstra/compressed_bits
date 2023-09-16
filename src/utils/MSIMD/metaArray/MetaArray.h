#ifndef METAARRAY
#define METAARRAY

#include <stdint.h>
#include <vector>

// TODO consolidate these ussages
#define REGISTER_SIZE 32
#define uintN_t uint32_t
#define BYTESIZE 8

struct ma {
    uint8_t bprim;
    uint8_t exception_count;
    uint8_t * meta_array;
};

struct array_bitsize {
    // base array
    uintN_t * arr;

    // this is for the end of the array according to the size of uintN_t
    uintN_t end_register = 0;

    // this is for any additional bits that have been used in the array past the end
    uintN_t bits_addon = 0;
    uintN_t size = 0;
};

// IMPORTANT: barr_block that is passed in needs to be sorted
class MetaArray {
public:
    
    // barr is the original array (in our case the delta encoded one) that will be compressed
    MetaArray(uintN_t * barr_block, unsigned int barr_block_size);
    ~MetaArray();
    int get_exception_size(); // in bytes
    int get_meta_size(); // in bytes
    int get_n_exception(int block_n, int index);

    // int exceptionblocks_end[REGISTER_SIZE];
    // TODO put in private and change tests accordingliny
    struct array_bitsize exceptionblocks[REGISTER_SIZE+1];
private:
    int generate_blocks();
    int generate_metadata(); 
    int find_exceptions(const uintN_t * barr_block, unsigned int barr_block_size); // records the size and number of exceptions
    int store_exception_counts(uintN_t * barr_block);
    int count_exceptions(int bprim); // will count the exceptions store in the exceptions count array
    int calculate_bprim(uintN_t * barr_block);
    int allocate_exception_blocks();
    int store_exception(uintN_t value);

    struct ma meta_array;

    // index represents the bit length and the value is the number of times that length occured
    int exception_counts[REGISTER_SIZE + 1];
    
    // I don't need exception staging because I have already counted the number of exceptions in the
    //  exceptions count array. I can use that to generate the needed place and put everything into
    //  its final area in the find_exceptions method
    // uintN_t exceptions_size; // size in bytes
    // uintN_t * exceptionblocks[REGISTER_SIZE];

    // this would have to change if the block size ever goes above 256
};

#endif
