OBJ_DIR = ../../../../../../obj/
UTILITIES_DIR = ../../../../../utils/

# -------------------
# define our includes
# -------------------
INCLUDES=-I$(UTILITIES_DIR)/bits/utils/c/mt

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES= bsearch.c
OBJECTS= $(SOURCES:.c=.o)
_EXT_OBJECTS=mt.o timer.o
EXT_OBJECTS=$(patsubst %,$(OBJ_DIR)/%,$(_EXT_OBJECTS))
BUILT_OBJECTS= $(patsubst %,$(OBJ_DIR)/%,$(OBJECTS))

all: $(BUILT_OBJECTS)

$(BUILT_OBJECTS): $(SOURCES)
	@echo "  * compiling" $(*F).c
	@$(CXX) -O3 -c -o $@ $(*F).c $(LDFLAGS) $(CXXFLAGS) $(INCLUDES) \
		$(LOCAL_INCLUDES)

$(EXT_OBJECTS):
	@$(MAKE) --no-print-directory -C -W $(INCLUDES)

clean:
	@echo "Cleaning up."
	@rm -f $(BUILT_OBJECTS)

.PHONY: clean


