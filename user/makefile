# Define the compiler and flags
CC = i686-elf-gcc
CFLAGS = -m32 -std=gnu99 -ffreestanding -DATHENX_LIBC -O2 -O0 -nostdlib -I./../include/ -I../libc/include -Ilibc/include -g
LDFLAGS = -T linker.ld -Ilibc/include
LIBS = libc/libc.a

# Define the target folder to compile
TARGET_FOLDER = ./programs

# List of source files to compile in the specified folder
SRC_FILES := $(wildcard $(TARGET_FOLDER)/*.c)

# Output binary name based on the target folder
OUT_FILE := $(TARGET_FOLDER).elf

# Target to build the program
all: $(OUT_FILE)

# Rule to build the program from its source files
$(OUT_FILE): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)
	cp $(OUT_FILE) ../sysroot/init
# Clean rule to remove the generated file
clean:
	rm -f $(OUT_FILE)

.PHONY: all clean
