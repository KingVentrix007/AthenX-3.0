# Define the compiler and flags
CC = i686-elf-gcc
CFLAGS = -m32 -std=gnu99 -ffreestanding -DATHENX_LIBC -O2 -O0 -nostdlib -I./../include/ -I../libc/include -Ilibc/include -g
LDFLAGS = -Ilibc/include linker.ld
LIBS = ../libc/out/libc.a

# Define the directory to copy the ELF files to
BIN_DIR = ../sysroot/bin

# Define the target folders to compile
TARGET_FOLDERS = $(wildcard ./programs/*)

# Generate a list of output binaries based on the target folders
OUT_FILES = $(patsubst %, %.elf, $(TARGET_FOLDERS))

# Target to build all programs
all: $(OUT_FILES) copy
	@echo "Compilation successful."

# Rule to build each program from its source files in each target folder
%.elf: %/*.c
	@echo "Compiling $^"  # Print the list of source files being compiled
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

# Rule to copy each ELF file to the binary directory
copy: $(OUT_FILES)
	@mkdir -p $(BIN_DIR)
	@cp $(OUT_FILES) $(BIN_DIR)
	@rm $(OUT_FILES)

# Clean rule to remove the generated files
clean:
	rm -f $(OUT_FILES)

# Mark the .elf files as phony targets to prevent recompilation
.PHONY: all clean copy
