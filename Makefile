# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -std=c23 -pedantic -g -O0
LDFLAGS = 
LIBS = 

# Directories
SRC_DIR = src
TEST_DIR = tests
OBJ_DIR = obj
BIN_DIR = bin

# Project name
PROJECT = string_buffer

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test files
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_BINARIES = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(BIN_DIR)/%)

# Main targets
TARGET = $(BIN_DIR)/$(PROJECT)
TEST_TARGET = $(BIN_DIR)/test_runner

# Default target
all: $(TARGET)

# Create directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build main executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

# Build object files from source
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test object files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Build individual test binaries
$(BIN_DIR)/%: $(TEST_DIR)/%.c $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS)) | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $^ -o $@ $(LIBS)

# Build and run tests
test: $(TEST_BINARIES)
	@echo "Running tests..."
	@for test in $(TEST_BINARIES); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo "All tests passed!"

# Build test runner (if you have a unified test runner)
$(TEST_TARGET): $(TEST_OBJECTS) $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS)) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

# Run main program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -g3 -O0
debug: $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG -O2
release: clean $(TARGET)

# Static analysis
analyze: $(SOURCES)
	clang-tidy $(SOURCES) -- $(CFLAGS) -I$(SRC_DIR)

# Format code
format:
	clang-format --style=LLVM -i $(SRC_DIR)/*.c $(SRC_DIR)/*.h $(TEST_DIR)/*.c

# Generate dependencies
deps: $(SOURCES)
	$(CC) -MM $(SOURCES) > .depend

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Install (example)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(PROJECT)

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the main executable (default)"
	@echo "  test     - Build and run all tests"
	@echo "  run      - Build and run the main program"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized release version"
	@echo "  analyze  - Run static analysis with clang-tidy"
	@echo "  format   - Format code with clang-format"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install the program"
	@echo "  help     - Show this help message"

# Phony targets
.PHONY: all test run debug release analyze format clean install uninstall help

# Include dependencies if available
-include .depend
