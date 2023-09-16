add_subdirectory(s4-fastpfor)
add_subdirectory(delta)
add_subdirectory(metaArray)

add_library(msimd msimd.cpp msimd.h)
set_target_properties(msimd PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
)

target_link_libraries(msimd
    delta
    s4-fastpfor
    metaArray)