add_library(metaArray MetaArray.h MetaArray.cpp)

set_target_properties(metaArray PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
)

target_link_libraries(metaArray
    s4-fastpfor
)
target_include_directories(metaArray PUBLIC
    ../s4-fastpfor
)