CC = i686-elf-gcc
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -Iinclude

SRC_DIR = ./
BUILD_DIR = obj
DEST_DIR = ../../sysroot/usr/source

# List all source files in the SRC_DIR
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding object files in the BUILD_DIR
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Name of the resulting library
LIB = libc.a

# Compile and create the static library
$(LIB): $(OBJ_FILES)
	ar rcs $@ $^

# Compile each source file to an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories
$(BUILD_DIR) $(DEST_DIR):
	mkdir -p $@

# Copy source files to destination directory
$(DEST_DIR)/%.c: $(SRC_DIR)/%.c | $(DEST_DIR)
	cp $< $@

# Rule to ensure that the BUILD_DIR is created before compiling
$(OBJ_FILES): | $(BUILD_DIR)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(LIB)

.PHONY: clean
