add_library(s4-fastpfor s4-fastpfor.h s4-fastpfor.cpp)

set_target_properties(s4-fastpfor PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
)