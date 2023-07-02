add_library(gzstream SHARED gzstream.cpp gzstream.h)


find_package(ZLIB REQUIRED)
target_link_libraries(gzstream ZLIB::ZLIB)