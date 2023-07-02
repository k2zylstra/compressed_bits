add_library(genomeFile SHARED genomeFile.cpp genomeFile.h)

target_link_libraries(genomeFile lineFileUtilities)
target_include_directories(genomeFile PUBLIC ../lineFileUtilities)