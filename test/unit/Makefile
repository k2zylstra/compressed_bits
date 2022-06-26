UTILITIES_DIR = ../../src/utils/
OBJ_DIR = ../../obj/
BIN_DIR = ../../bin/

# -------------------
# define our includes
# -------------------
INCLUDES = -I$(UTILITIES_DIR)/bedFile/ \
           -I$(UTILITIES_DIR)/version/ \
           -I$(UTILITIES_DIR)/gzstream/ \
           -I$(UTILITIES_DIR)/genomeFile/ \
           -I$(UTILITIES_DIR)/lineFileUtilities/ \
           -I$(UTILITIES_DIR)/fileType/ \
		   -I$(UTILITIES_DIR)/varint/ \
	   	   -I$(UTILITIES_DIR)/delta/ \
           -I$(UTILITIES_DIR)/bits/interval_intersection/lib/seq/ \
	   -I$(UTILITIES_DIR)/bits/utils/c/mt/ \
	   -I$(UTILITIES_DIR)/bits/utils/c/timer/ \
	   -I$(UTILITIES_DIR)/bits/utils/c/file_read/ \
# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES= test_deltas.cpp
OBJECTS= $(SOURCES:.cpp=.o)
_EXT_OBJECTS=bedFile.o \
	     lineFileUtilities.o \
	     gzstream.o \
	     fileType.o \
		 varint.o	\
		 delta.o \
	     interval.o \
	     bsearch.o \
	     mt.o \
	     timer.o \
	     file_read.o \
	     genomeFile.o 
EXT_OBJECTS=$(patsubst %,$(OBJ_DIR)/%,$(_EXT_OBJECTS))
BUILT_OBJECTS= $(patsubst %,$(OBJ_DIR)/%,$(OBJECTS))
PROGRAM= test_delta 


all: $(PROGRAM)

.PHONY: all

$(PROGRAM): $(BUILT_OBJECTS) $(EXT_OBJECTS)
	@echo $(SOURCES)
	@echo " * linking $(PROGRAM)"
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $(BIN_DIR)/$@ $^ $(LIBS) \
		$(LOCAL_LIBS) -lz -lgsl -lgslcblas -lm

$(BUILT_OBJECTS): $(SOURCES)
	@echo " * compiling" $(*F).cpp
	@$(CXX) -g -c -o $@ $(*F).cpp $(LDFLAGS) $(CXXFLAGS) $(INCLUDES) \
		$(LOCAL_INCLUDES) -lgsl -lgslcblas
	@echo "----------------------"

$(EXT_OBJECTS):
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/lineFileUtilities/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/bedFile/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/gzstream/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/genomeFile/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/fileType/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/bits/interval_intersection/lib/seq/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/bits/binary_search/lib/seq/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/bits/utils/c/mt/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/bits/utils/c/timer/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/bits/utils/c/file_read/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/varint/
	@$(MAKE) --no-print-directory -C $(UTILITIES_DIR)/delta/

clean:
	@echo "Cleaning up."
	@rm -f $(EXT_OBJECTS) $(BUILT_OBJECTS)

.PHONY: clean
