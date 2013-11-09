# Set up source and destination directories

TARGET  = test
BIN_DIR = bin/
OBJ_DIR = obj/
CPP_DIR = src/
HPP_DIR = include/

TGT_OBJ_DIR = ../obj/
TGT_HPP_DIR = ../include/

# Define the default compiler settings

CXXFLAGS = -O3 -Wall -Wextra -std=c++11
LDFLAGS = -lgtest -pthread

# Locate relevant source files, headers, and destination .obj's

BIN = $(addprefix $(BIN_DIR),$(TARGET))
HPP = $(shell find $(HPP_DIR) -name '*.hpp')
CPP = $(shell find $(CPP_DIR) -name '*.cpp')
OBJ = $(subst .cpp,.o,$(subst $(CPP_DIR),$(OBJ_DIR),$(CPP)))

TGT_HPP = $(shell find $(TGT_HPP_DIR) -name '*.hpp')
TGT_OBJ = $(shell find $(TGT_OBJ_DIR) -type f \( -name '*.o' ! -name 'main.o' \))

# Compile the target program

default: $(BIN)

run: $(BIN)
	$(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJ) $(TGT_OBJ) -o $(BIN) $(LDFLAGS)

$(OBJ): $(OBJ_DIR)%.o: $(CPP_DIR)%.cpp $(HPP) $(TGT_HPP)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(HPP_DIR) -I$(TGT_HPP_DIR) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

