add_library(bsearch SHARED bsearch.c bsearch.h)
target_link_libraries(bsearch mt)
target_include_directories(bsearch PUBLIC ../../../utils/c/mt)