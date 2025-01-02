#Makefile
# Created on: Jan 2, 2025
#     Author: swalton

BIN_DIR=./bin
CXXFLAGS=-std=c++14 -Wall -g3 -Wno-multichar

SRC_CPP=$(wildcard ./*.cpp)
OBJ_CPP=$(patsubst %.cpp,$(BIN_DIR)/%.o,$(SRC_CPP))

$(BIN_DIR)/main: $(BIN_DIR) $(OBJ_CPP)
	g++ -o $@ $(OBJ_CPP)

$(BIN_DIR):
	mkdir -p $@

$(OBJ_CPP): $(BIN_DIR)/%.o: %.cpp
	g++ -c $(CXXFLAGS) $< -o $@
	
clean:
	@rm -rf $(BIN_DIR)