# Set up source and destination directories

TARGET  = voxel
BIN_DIR = bin/
OBJ_DIR = obj/
CPP_DIR = src/
HPP_DIR = include/
TEST_DIR = test/
TMP_DIR = /tmp/

# Define the default compiler settings

CXX ?= clang++
CXXFLAGS = -O3 -Wall -Wextra -std=c++11

LDFLAGS = 
TEST_LDFLAGS = -lgtest -pthread

# Locate relevant source files, headers, and destination .obj's

TMP = $(addprefix $(TMP_DIR),$(TARGET))
BIN = $(addprefix $(BIN_DIR),$(TARGET))
HPP = $(shell find $(HPP_DIR) -name '*.hpp')
CPP = $(shell find $(CPP_DIR) -name '*.cpp')
OBJ = $(subst .cpp,.o,$(subst $(CPP_DIR),$(OBJ_DIR),$(CPP)))

TEST_OBJ = $(shell find $(OBJ_DIR) -type f \( -name '*.o' ! -name 'main.o' \))
TEST_CPP = $(shell find $(TEST_DIR) -name '*.cpp')

# Compile the target program

default: $(BIN)

run: $(BIN)
	$(BIN)

$(BIN): precompile $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJ) -o $(BIN) $(LDFLAGS)

$(OBJ): $(OBJ_DIR)%.o: $(CPP_DIR)%.cpp $(HPP)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(HPP_DIR) -I../../ -c $< -o $@

# Assign defaults to configs

renderer ?= RendererGI

# The precompile pass checks to see if any changes should be made to the compile_settings
# file. If this file must be changed, a recompile is induced.

check_setting = $(shell if test "$(shell sed -n '$(2)P' include/compile_settings.hpp)" != "typedef $(1) $(3);"; then (sed -i "$(2)s/.*/typedef $(1) $(3)\;/" include/compile_settings.hpp); fi)

.PHONY: precompile
precompile:
	$(call check_setting,$(renderer),3,"Renderer")

.PHONY: test
test: $(OBJ) $(HPP)
	$(CXX) $(CXXFLAGS) -I$(HPP_DIR) -I../../ $(TEST_OBJ) $(TEST_CPP) -o $(TMP) $(LDFLAGS) $(TEST_LDFLAGS)
	-$(TMP)
	rm $(TMP)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

