#include "MetaArray.h"

#include <cmath>
#include <cstring>

#include "s4-fastpfor.h"

/*
I could have the meta array be stored in a vector as it shouldn't be too big and
there is supposed to be one per MISMD, rather than per block. The vector will
just reallocate memory and will double itself. the other option is to store it
in an array and have the array size to be set for a few blocks, but there will
be left over space as the information wont fit perfectly into whatever block I
have. I could pass in the entire barr, but then I couldn't do the delta encoding
by pieces and would always need two massive arrays in memory. I will do one meta
array per block, but keep the bits array for everything. That means MSIMD needs
to create the storage space for the numbers and should pass them into the block
metadata does need to be unique per block because it has the b prim value in it.
We could store it every so often but that would invite other challenges such as
finding the correct bprim
*/

// the exceptions array needs to be n x 32
MetaArray::MetaArray(uintN_t* barr_block, unsigned int barr_block_size) {
  // calculate bprim
  // TODO this assumes size of 256 while find_exceptions does not
  MetaArray::calculate_bprim(barr_block);

  // generate place for exceptions
  MetaArray::allocate_exception_blocks();

  // generate place for metadata array
  MetaArray::meta_array.meta_array =
      (uint8_t*)malloc((MetaArray::meta_array.exception_count * BYTESIZE) + BYTESIZE + BYTESIZE);

  // store exceptions
  MetaArray::find_exceptions(barr_block, barr_block_size);
}

MetaArray::~MetaArray() {
  for (int i = 1; i < REGISTER_SIZE + 1; i++) {
    free(MetaArray::exceptionblocks[i].arr);
  }
}

int MetaArray::allocate_exception_blocks() {
  for (int i = 1; i < REGISTER_SIZE + 1; i++) {
    MetaArray::exceptionblocks[i].arr = (uintN_t*)calloc(MetaArray::exception_counts[i], (i + 1));
  }

  return 0;
}

int MetaArray::calculate_bprim(uintN_t* barr_block) {
  int bprim_tmp;
  int max = INT32_MAX;
  int max_new;
  int total_exceptions_count;

  MetaArray::store_exception_counts(barr_block);

  // should be the last element (largest) in sorted array
  int used_bits = calculate_bits_used_debruijan_32(barr_block[BLOCKSIZE - 1]);

  for (bprim_tmp = 0; bprim_tmp < REGISTER_SIZE; bprim_tmp++) {
    max_new = (BLOCKSIZE * bprim_tmp) + (MetaArray::count_exceptions(bprim_tmp) * (used_bits - bprim_tmp + BYTESIZE));
    if (max_new < max) {
      max = max_new;
      MetaArray::meta_array.bprim = bprim_tmp;
    }
  }

  MetaArray::meta_array.exception_count = MetaArray::count_exceptions(MetaArray::meta_array.bprim);
  return MetaArray::meta_array.bprim;
}

int MetaArray::store_exception_counts(uintN_t* barr_block) {
  memset(MetaArray::exception_counts, 0, REGISTER_SIZE);

  for (int i = 0; i < BLOCKSIZE; i++) {
    ++MetaArray::exception_counts[calculate_bits_used_debruijan_32(barr_block[i])];
  }
  return 0;
}

int MetaArray::count_exceptions(int bprim_tmp) {
  int count = 0;
  for (int i = bprim_tmp - 1; i < REGISTER_SIZE; i++) {
    count += MetaArray::exception_counts[i];
  }
  return count;
}

/*
this will find the number of exceptions and calculate the size of the needed
meta array is there a way that I can iterate through the block only once and
grab all the exceptions? I am already iterating through to count, but I will not
know the size of what I am counting until I total it up I can either
have
MetaArray::exceptionblocks[i].arr[MetaArray::exceptionblocks[i].end_register]
going block by block and would also save the time of going through the list
completely a second time and would only reiterate over the ones again. However,
it would mean having to restore the items a second time. This may need to be
tested empircally to be verified if it does save time, versus actually just
going through the full list twice. Each store will have to move a value into an
address in RAM, which will change in speed based on caching algorithms. I am not
sure why I need the b value to be stored in the MetaArray block. All I need is
the bprim value and I can reconstruct the value based on which exceptions array
it is in
*/

int MetaArray::find_exceptions(const uintN_t* barr_block, unsigned int barr_block_size) {
  int index;
  int max_value = 0x1 << MetaArray::meta_array.bprim;
  uintN_t value;
  uintN_t mask = 0x1;

  // this should make a mask of all ones except for the bprim numbers
  // starts at one fore since mask is already 1
  for (index = 1; index < meta_array.bprim; index++) {
    mask |= (mask << 1);
  }
  mask = ~mask;

  for (index = 0; index < barr_block_size; index++) {
    value = barr_block[index];
    if (value >= max_value) {
      MetaArray::store_exception(value);
    }
  }

  return 0;
}

int MetaArray::store_exception(uintN_t value) {
  uint tmp;
  uint index;
  uint exception_bits;
  uint overflow_bits_count;
  uint isolate_shift_number;
  uint exception_bits_count;
  uint bits_left_in_register;

  exception_bits = value >> MetaArray::meta_array.bprim;
  exception_bits_count = calculate_bits_used_debruijan_32(exception_bits);
  bits_left_in_register = REGISTER_SIZE - MetaArray::exceptionblocks[exception_bits_count].bits_addon;
  overflow_bits_count = exception_bits_count - bits_left_in_register;
  index = exception_bits_count;

  if (exception_bits_count > bits_left_in_register) {
    isolate_shift_number = REGISTER_SIZE - overflow_bits_count;
    tmp = exception_bits << isolate_shift_number;
    exception_bits >>= overflow_bits_count;
    MetaArray::exceptionblocks[index].arr[MetaArray::exceptionblocks[index].end_register] |= exception_bits;
    ++MetaArray::exceptionblocks[index].end_register;
    MetaArray::exceptionblocks[index].arr[MetaArray::exceptionblocks[index].end_register] |= tmp;
    MetaArray::exceptionblocks[index].bits_addon = overflow_bits_count;
  } else {
    exception_bits <<= bits_left_in_register - exception_bits_count;
    MetaArray::exceptionblocks[index].arr[MetaArray::exceptionblocks[index].end_register] |= exception_bits;
    MetaArray::exceptionblocks[index].bits_addon += exception_bits_count;
  }

  return 0;
}