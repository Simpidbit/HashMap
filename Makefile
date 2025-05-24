# Makefile for HashMap project

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I.
DEBUG_FLAGS = -g -DDEBUG
UTILS_DIR = utils

# Source files
SOURCES = test_hashmap.cpp
HEADERS = hashmap.hpp $(UTILS_DIR)/xxhash32.hpp $(UTILS_DIR)/rbtree.hpp $(UTILS_DIR)/bitmap.hpp $(UTILS_DIR)/__def.hpp $(UTILS_DIR)/__errs.hpp $(UTILS_DIR)/__iterator.hpp

# Output binaries
TARGET = test_hashmap
STL_TARGET = test_stl_compliance
EXPANSION_TARGET = test_expansion
SIMPLE_TARGET = simple_test
XXHASH_TARGET = test_xxhash

# Default target
all: $(TARGET) $(STL_TARGET) $(EXPANSION_TARGET) $(SIMPLE_TARGET) $(XXHASH_TARGET)

# Build the main test program
$(TARGET): test_hashmap.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) test_hashmap.cpp

# Build the STL compliance test
$(STL_TARGET): test_stl_compliance.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(STL_TARGET) test_stl_compliance.cpp

# Build the expansion test
$(EXPANSION_TARGET): test_expansion.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(EXPANSION_TARGET) test_expansion.cpp

# Build the simple test
$(SIMPLE_TARGET): simple_test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(SIMPLE_TARGET) simple_test.cpp

# Build the xxhash test
$(XXHASH_TARGET): test_xxhash.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(XXHASH_TARGET) test_xxhash.cpp

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET) $(STL_TARGET) $(EXPANSION_TARGET) $(SIMPLE_TARGET) $(XXHASH_TARGET)

# Run main tests
test: $(TARGET)
	./$(TARGET)

# Run STL compliance tests
test-stl: $(STL_TARGET)
	./$(STL_TARGET)

# Run all tests
test-all: $(TARGET) $(STL_TARGET) $(EXPANSION_TARGET) $(SIMPLE_TARGET)
	@echo "Running main tests..."
	./$(TARGET)
	@echo "Running STL compliance tests..."
	./$(STL_TARGET)
	@echo "Running expansion tests..."
	./$(EXPANSION_TARGET)
	@echo "Running simple tests..."
	./$(SIMPLE_TARGET)

# Check for memory leaks (requires valgrind)
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Format code (requires clang-format)
format:
	clang-format -i *.cpp *.hpp

# Generate documentation (requires doxygen)
docs:
	cd $(UTILS_DIR) && doxygen Doxyfile

.PHONY: all debug clean test test-stl test-all valgrind format docs
