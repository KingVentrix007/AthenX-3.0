# Makefile for libc

CC = i686-elf-gcc
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -Iinclude

SRC_DIR = ./
BUILD_DIR = obj
DEST_DIR = dest

# Recursively find all source files in the SRC_DIR
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.c')

# Generate corresponding object files in the BUILD_DIR
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Name of the resulting library
LIB = libc.a

# Compile and create the static library
$(LIB): $(OBJ_FILES)
	ar rcs $@ $^

# Compile each source file to an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create destination directory if it doesn't exist
$(DEST_DIR):
	mkdir -p $(DEST_DIR)

# Copy each source file to the destination directory
$(DEST_DIR)/%.c: $(SRC_DIR)/%.c | $(DEST_DIR)
	cp $^ $@

# Rule for cleaning up build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o $(LIB) $(DEST_DIR)
