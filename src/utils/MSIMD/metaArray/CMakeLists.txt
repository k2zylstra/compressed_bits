add_library(metaArray MetaArray.h MetaArray.cpp)

target_link_libraries(metaArray
    s4-fastpfor
)
target_include_directories(metaArray PUBLIC
    ../s4-fastpfor
)