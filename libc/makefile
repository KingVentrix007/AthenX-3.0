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

# Copy the contents of the include directory to ../user/include
copy_include:
	cp -r include/* ../user/include

# Copy libc.a to ../user/libc.a
copy_libc:
	cp $(LIB) ../user/$(LIB)

# Phony target to build the library and copy necessary files
all: $(LIB) copy_include copy_libc

# Rule for cleaning up build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o $(LIB) $(DEST_DIR) ../user/include ../user/$(LIB)

# Custom target to compile ANSI library
ansi:
	$(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" SRC_DIR="ansi" all

# Custom target to compile POSIX library
posix:
	$(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" SRC_DIR="posix" all
.PHONY: posix ansi