include(FetchContent)
FetchContent_Declare(
    googletest
    URL  https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip

)

FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(test_s4fastpfor test_s4fastpfor.cpp)
target_link_libraries(test_s4fastpfor
    s4-fastpfor
    # for whatever reason, linking this library messes with the google test entrypoints
    mt
    timer
    GTest::gtest_main
    GTest::gmock_main
)
target_include_directories(test_s4fastpfor PUBLIC
    ../../utils/MSIMD/s4-fastpfor
    ../../utils/bits/utils/c/mt
    ../../utils/bits/utils/c/timer
)

add_executable(test_bits_count test_bits_count.cpp)
target_link_libraries(test_bits_count
    interval
    bedFile
    genomeFile
    lineFileUtilities
    file_read
    GTest::gtest_main
)
target_include_directories(test_bits_count PUBLIC
    ../../utils/bits/interval_intersection/lib/seq/
    ../../utils/bedFile
    ../../utils/genomeFile
    ../../utils/lineFileUtilities
    ../../utils/file_read
)

add_executable(test_metaarray test_metaarray.cpp)
target_link_libraries(test_metaarray PUBLIC
    metaArray
    GTest::gtest_main
)
target_include_directories(test_metaarray PUBLIC
    ../../utils/MSIMD/metaArray/
)

gtest_discover_tests(test_metaarray)
gtest_discover_tests(test_bits_count)
gtest_discover_tests(test_s4fastpfor)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../data/HSMMtube_Skeletal_Muscle_Myotubes_Derived_from_HSMM_Active_TSS.bed
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../data/HSMMtube_Skeletal_Muscle_Myotubes_Derived_from_HSMM_Enhancers.bed
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../data/human.hg38.genome
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)