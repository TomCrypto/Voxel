# Set up source and destination directories

TARGET  = voxel
BIN_DIR = bin/
OBJ_DIR = obj/
CPP_DIR = src/
HPP_DIR = include/
TEST_DIR = test/

# Define the default compiler settings

CXX = clang++
CXXFLAGS = -O3 -Wall -Wextra -std=c++11

# Locate relevant source files, headers, and destination .obj's

BIN = $(addprefix $(BIN_DIR),$(TARGET))
HPP = $(shell find $(HPP_DIR) -name '*.hpp')
CPP = $(shell find $(CPP_DIR) -name '*.cpp')
OBJ = $(subst .cpp,.o,$(subst $(CPP_DIR),$(OBJ_DIR),$(CPP)))

# Compile the target program

default: $(BIN)

run: $(BIN)
	$(BIN)

$(BIN): precompile $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) $(OBJ) -o $(BIN)

$(OBJ): $(OBJ_DIR)%.o: $(CPP_DIR)%.cpp $(HPP)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(HPP_DIR) -c $< -o $@

# Assign defaults to configs

RENDERER = RendererGI

.PHONY: precompile
precompile:
	@if test $(shell echo -n "\""; cat include/compile_settings.hpp | grep -i "Renderer"; echo -n "\"") != $(shell printf "\"typedef $(RENDERER) Renderer; \""); then (sed -i '3s/.*/typedef $(RENDERER) Renderer;/' include/compile_settings.hpp); fi
.PHONY: test
test:
	cd $(TEST_DIR); $(MAKE) && $(MAKE) run

.PHONY: clean
clean:
	cd $(TEST_DIR); $(MAKE) clean
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

