add_library(bedFile SHARED bedFile.cpp bedFile.h)

target_link_libraries(bedFile gzstream)
target_link_libraries(bedFile lineFileUtilities)
target_link_libraries(bedFile fileType)
target_link_libraries(bedFile interval)
target_link_libraries(bedFile timer)

target_include_directories(bedFile PUBLIC ../gzstream)
target_include_directories(bedFile PUBLIC ../lineFileUtilities)
target_include_directories(bedFile PUBLIC ../fileType)
target_include_directories(bedFile PUBLIC ../bits/interval_intersection/lib/seq)
target_include_directories(bedFile PUBLIC ../bits/utils/c/timer)

