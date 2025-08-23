# Adapter Makefile

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./include
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BUILD_DIR = build

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(BUILD_DIR)/main.o
LIB_OBJECTS = $(filter-out $(MAIN_OBJ), $(OBJECTS))

# Test files
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/test_%.o)
TEST_EXECUTABLES = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/test_%)

# Target executable
TARGET = $(BUILD_DIR)/adapter

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build main executable
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built executable: $(TARGET)"

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build test object files
$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build test executables
$(BUILD_DIR)/test_%: $(BUILD_DIR)/test_%.o $(LIB_OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Build and run tests
test: $(TEST_EXECUTABLES)
	@echo "Running tests..."
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running $$test"; \
		$$test || exit 1; \
	done
	@echo "All tests passed!"

# Install target (copy to /usr/local/bin)
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin/"
	@sudo cp $(TARGET) /usr/local/bin/adapter
	@sudo chmod +x /usr/local/bin/adapter
	@echo "Installation complete!"

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Debug build
debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(TARGET)

# Release build (optimized)
release: CXXFLAGS += -DNDEBUG -O3 -march=native
release: clean $(TARGET)

# Show help
help:
	@echo "Adapter Makefile"
	@echo "Available targets:"
	@echo "  all      - Build the main executable (default)"
	@echo "  test     - Build and run all tests"
	@echo "  clean    - Remove all build files"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this help message"

# Check code format (requires clang-format)
format:
	@echo "Formatting code..."
	@find $(SRC_DIR) $(INCLUDE_DIR) $(TEST_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i --style=LLVM
	@echo "Code formatting complete!"

# Static analysis (requires cppcheck)
analyze:
	@echo "Running static analysis..."
	@cppcheck --enable=all --std=c++17 -I$(INCLUDE_DIR) $(SRC_DIR)

# Show project structure
structure:
	@echo "Project structure:"
	@tree -I build

.PHONY: all test clean debug release install help format analyze structure
