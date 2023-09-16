cmake_policy(SET CMP0048 NEW)

project("Compressed Bits" VERSION 1.0.0)
cmake_minimum_required(VERSION 3.12)

project(comp_bits VERSION 3.12)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--config-file=${CMAKE_CURRENT_SOURCE_DIR}/../.clang-tidy")
find_program(CLANG_FORMAT_EXE NAMES "clang-format")
set(CLANG_FORMAT_COMMAND "${CLANG_TIDY_EXEC}" "--style=Google" "--dry-run")

include_directories(utils/version)

add_subdirectory(utils)
add_subdirectory(test/unit)

# generate the executables
add_executable(bits_count bits_count/bits_count_main.cpp bits_count/bits_count.cpp bits_count/bits_count.h)
set_target_properties(bits_count PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)
#add_executable(bits_test bits_test/bits_test_main.cpp bits_test/bits_test.cpp bits_test/bits_test.h)


# added dependencies for bits_count
target_include_directories(bits_count PUBLIC utils/bits/interval/lib/seq
                                      utils/bits/utils/c/timer
                                      utils/lineFileUtilities
                                      utils/bedFile
                                      utils/genomeFile
                                      utils/file_read)

target_link_libraries(bits_count timer
                                 genomeFile
                                 lineFileUtilities
                                 interval
                                 bedFile
                                 file_read)


# added dependencies for bits_test
# target_include_directories(bits_test PUBLIC utils/bits/interval/lib/seq)
# target_include_directories(bits_test PUBLIC utils/bits/utils/c/timer)
# target_include_directories(bits_test PUBLIC utils/lineFileUtilities)
# target_include_directories(bits_test PUBLIC utils/bedFile)
# target_include_directories(bits_test PUBLIC utils/genomeFile)
# target_include_directories(bits_test PUBLIC utils/file_read)

# target_link_libraries(bits_test timer)
# target_link_libraries(bits_test genomeFile)
# target_link_libraries(bits_test lineFileUtilities)
# target_link_libraries(bits_test interval)
# target_link_libraries(bits_test bedFile)
# target_link_libraries(bits_test file_read)
