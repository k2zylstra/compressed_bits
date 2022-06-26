export OBJ_DIR	= obj
export BIN_DIR	= bin
export SRC_DIR	= src
export TEST_DIR = test/unit
export CXX		= g++
export CXXFLAGS = -Wall -O2 -g -fPIC
export LIBS		= -lz

SUBDIRS = $(SRC_DIR)/bits_count \
          $(SRC_DIR)/bits_test \
		  $(TEST_DIR)

UTIL_SUBDIRS =	$(SRC_DIR)/utils/lineFileUtilities \
		$(SRC_DIR)/utils/bedFile \
		$(SRC_DIR)/utils/genomeFile \
		$(SRC_DIR)/utils/gzstream \
		$(SRC_DIR)/utils/fileType \
		$(SRC_DIR)/utils/bits/utils/c/mt \
		$(SRC_DIR)/utils/varint \
		$(SRC_DIR)/utils/delta \
		$(SRC_DIR)/utils/bits/utils/c/timer \
		$(SRC_DIR)/utils/bits/utils/c/file_read \
		$(SRC_DIR)/utils/bits/binary_search/lib/seq \
		$(SRC_DIR)/utils/bits/interval_intersection/lib/seq

all:
	[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	
	@echo "Building BITS:"
	@echo "========================================================="
	
	@for dir in $(UTIL_SUBDIRS); do \
		echo "- Building in $$dir"; \
		$(MAKE) --no-print-directory -C $$dir; \
		echo ""; \
	done

	@for dir in $(SUBDIRS); do \
		echo "- Building in $$dir" SUBDIRS; \
		$(MAKE) --no-print-directory -C $$dir; \
		echo ""; \
	done


.PHONY: all

clean:
	@echo "Cleaning up."
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/*

.PHONY: clean
