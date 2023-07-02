add_library(interval SHARED interval.c interval.h )


find_package(GSL REQUIRED)    # See below (2)
target_link_libraries(interval GSL::gsl GSL::gslcblas)

target_link_libraries(interval mt)
target_link_libraries(interval bsearch)
target_link_libraries(interval timer)
target_include_directories(interval PUBLIC ../../../utils/c/mt)
target_include_directories(interval PUBLIC ../../../utils/c/timer)
target_include_directories(interval PUBLIC ../../../binary_search/lib/seq)
