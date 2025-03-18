# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude -O2
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

# Directories
SRC_DIRS = src/core src/game
BUILD_DIR = build
DOCS_DIR = docs
EXEC_NAME = starfall

# Find all .c files and generate object file names
SRC = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c)) src/main.c
OBJ = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Final executable
EXEC = $(BUILD_DIR)/$(EXEC_NAME)

# Targets
.PHONY: all clean run docs

all: $(EXEC)

$(EXEC): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	@echo "Build complete: $(EXEC)"
	@# Copy assets after a successful build
	@echo "Copying assets..."
	@cp -r assets $(BUILD_DIR)/

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(EXEC)

# Format source code using clang-format
format:
	@echo "Formatting source and header files..."
	@clang-format -i $(SRC) $(HEADERS)

# Target to generate Doxygen documentation
docs: Doxyfile
	@echo "Generating Doxygen documentation..."
	@doxygen Doxyfile
	@echo "Documentation generated in $(DOCS_DIR)/html"

# Target to clean build files and documentation
clean:
	@echo "Cleaning build and documentation files..."
	@rm -rf $(BUILD_DIR) $(DOCS_DIR)
