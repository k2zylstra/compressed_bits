# mt_test.c is not added because the tests should be incorperated into google test
#   otherwise it makes a bunch of bogus entrypoints
add_library(mt SHARED mt.c mt.h)