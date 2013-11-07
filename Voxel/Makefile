BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INCLUDE = include
BINARY  = voxel

# Default compiler
CXX ?= g++

# Base compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11

# Chooses the proper compiler flags to use based on debug
# or release. By default release, use `make debug=1 ...`.
ifeq ($(debug), 1)
	CXXFLAGS += -O0 -ggdb -D DEBUG
else
	CXXFLAGS += -O3
endif

# Adds the -no-integrated-as flag if using clang
ifeq ($(CXX), clang)
    CXXFLAGS += -no-integrated-as
endif

# Disables any development features.
ifeq ($(lean), 1)
	CXXFLAGS += -D LEAN
endif

CFLAGS += $(extra)

HEADERS = $(shell find $(INCLUDE)/ -name '*.hpp')
SRC = $(shell find $(SRC_DIR)/ -name '*.cpp')
ASM = $(shell find $(SRC_DIR)/ -name '*.S')
SRCOBJ = $(subst .cpp,.cpp.o,$(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(SRC)))
ASMOBJ = $(subst .S,.S.o,$(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(ASM)))
BINPATH = $(addprefix $(BIN_DIR)/, $(BINARY))

default: $(OBJ_DIR) $(BIN_DIR) $(BINPATH)

$(OBJ_DIR):
	@mkdir $@

$(BIN_DIR):
	@mkdir $@

$(BINPATH): $(SRCOBJ) $(ASMOBJ)
	$(CXX) $(LDFLAGS) $(SRCOBJ) $(ASMOBJ) -o $(BINPATH)

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@

$(OBJ_DIR)/%.S.o: $(SRC_DIR)/%.S $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

