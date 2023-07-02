project("Compressed Bits")
cmake_minimum_required(VERSION 3.12)

project(comp_bits)


include_directories(utils/version)

add_subdirectory(utils)

add_executable(bits_count bits_count/bits_count_main.cpp bits_count/bits_count.cpp bits_count/bits_count.h)
add_executable(bits_test bits_test/bits_test_main.cpp bits_test/bits_test.cpp bits_test/bits_test.h)





target_include_directories(bits_count PUBLIC utils/bits/interval/lib/seq)
target_include_directories(bits_count PUBLIC utils/bits/utils/c/timer)
target_include_directories(bits_count PUBLIC utils/lineFileUtilities)
target_include_directories(bits_count PUBLIC utils/bedFile)
target_include_directories(bits_count PUBLIC utils/genomeFile)
target_include_directories(bits_count PUBLIC utils/file_read)

target_link_libraries(bits_count timer)
target_link_libraries(bits_count genomeFile)
target_link_libraries(bits_count lineFileUtilities)
target_link_libraries(bits_count interval)
target_link_libraries(bits_count bedFile)
target_link_libraries(bits_count file_read)


target_include_directories(bits_test PUBLIC utils/bits/interval/lib/seq)
target_include_directories(bits_test PUBLIC utils/bits/utils/c/timer)
target_include_directories(bits_test PUBLIC utils/lineFileUtilities)
target_include_directories(bits_test PUBLIC utils/bedFile)
target_include_directories(bits_test PUBLIC utils/genomeFile)
target_include_directories(bits_test PUBLIC utils/file_read)

target_link_libraries(bits_test timer)
target_link_libraries(bits_test genomeFile)
target_link_libraries(bits_test lineFileUtilities)
target_link_libraries(bits_test interval)
target_link_libraries(bits_test bedFile)
target_link_libraries(bits_test file_read)


# target_link_libraries(bits_test interval)
# target_link_libraries(bits_count bedFile)
# target_link_libraries(bits_test bedFile)

# target_link_libraries(bits_count bsearch)
# target_link_libraries(bits_test bsearch)