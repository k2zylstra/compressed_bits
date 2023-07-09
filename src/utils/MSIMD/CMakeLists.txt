add_subdirectory(s4-fastpfor)
add_subdirectory(delta)
add_subdirectory(metaArray)

add_library(msimd msimd.cpp msimd.h)
target_link_libraries(msimd
    delta
    s4-fastpfor
    metaArray)