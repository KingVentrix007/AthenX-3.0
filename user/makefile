# Define the compiler and flags
CC = i686-elf-gcc
CFLAGS = -m32 -std=gnu99 -ffreestanding -DATHENX_LIBC -O2 -O0 -nostdlib -I./../include/ -I../libc/include -Ilibc/include -g
LDFLAGS = -Ilibc/include
LIBS = libc/libc.a

# Define the target folders to compile
TARGET_FOLDERS = $(wildcard ./programs/*)

# Generate a list of output binaries based on the target folders
OUT_FILES = $(patsubst %, %.elf, $(TARGET_FOLDERS))

# Target to build all programs
all: $(OUT_FILES)

# Rule to build each program from its source files in each target folder
%.elf: $(wildcard %/*.c)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)
	cp $@ ../sysroot/init

# Clean rule to remove the generated files
clean:
	rm -f $(OUT_FILES)

# Mark the .elf files as phony targets to prevent recompilation
.PHONY: all clean $(OUT_FILES)
