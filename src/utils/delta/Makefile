OBJ_DIR = ../../../obj/
BIN_DIR = ../../../bin/
UTILITIES_DIR = ../../utils/
# -------------------
# define our includes
# -------------------
INCLUDES = -I$(UTILITIES_DIR)/lineFileUtilities/ \
		   -I$(UTILITIES_DIR)/gzstream/ \
		   -I$(UTILITIES_DIR)/fileType/ \
		   -I$(UTILITIES_DIR)/stringUtilities/ \
		   -I$(UTILITIES_DIR)/bits/utils/c/timer/ \
           -I$(UTILITIES_DIR)/bits/interval_intersection/lib/seq/ \
		   -I$(UTILITIES_DIR)/varint/

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES= delta.cpp
OBJECTS= $(SOURCES:.cpp=.o)
_EXT_OBJECTS=lineFileUtilities.o gzstream.o fileType.o varint.o
EXT_OBJECTS=$(patsubst %,$(OBJ_DIR)/%,$(_EXT_OBJECTS))
BUILT_OBJECTS= $(patsubst %,$(OBJ_DIR)/%,$(OBJECTS))

$(BUILT_OBJECTS): $(SOURCES)
	@echo "  * compiling" $(*F).cpp
	@$(CXX) -g -c -o $@ $(*F).cpp $(LDFLAGS) $(CXXFLAGS) $(INCLUDES) 

$(EXT_OBJECTS):
	@$(MAKE) --no-print-directory -C -W $(INCLUDES)

clean:
	@echo "Cleaning up."
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/*

.PHONY: clean