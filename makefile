CC = i686-elf-gcc
AS = as
ASM = nasm
CONFIG = ./config
GCCPARAMS =  -O0 -I./include -fno-omit-frame-pointer -nostdlib -fno-pic -fno-builtin -fno-exceptions -ffreestanding -fno-leading-underscore -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long  -g -DINI_USE_STACK=0
	
ASPARAMS = --32
LDPARAMS = -m elf_i386 -T linker.ld -nostdlib 

OBJ_DIR = obj
SRC_DIRS = arch kernel drivers libk asm syscalls
OBJ_DIRS = $(addprefix $(OBJ_DIR)/,$(SRC_DIRS))

$(shell mkdir -p $(OBJ_DIRS))

# List of directories to be blacklisted
BLACKLIST_DIRS := elf
# Function to filter out blacklisted directories
define filter-out-dir
$(foreach dir,$(1),$(shell find . -type d -name '$(dir)'))
endef

# Update SRC_FILES_C, SRC_FILES_S, and SRC_FILES_ASM to exclude blacklisted directories
SRC_FILES_C := $(filter-out $(call filter-out-dir,$(BLACKLIST_DIRS)),$(shell find $(SRC_DIRS) -type f -name '*.c'))
SRC_FILES_S := $(filter-out $(call filter-out-dir,$(BLACKLIST_DIRS)),$(shell find $(SRC_DIRS) -type f -name '*.s'))
SRC_FILES_ASM := $(filter-out $(call filter-out-dir,$(BLACKLIST_DIRS)),$(shell find $(SRC_DIRS) -type f -name '*.asm'))

# Generate object file paths
OBJ_FILES_C := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC_FILES_C))
OBJ_FILES_S := $(patsubst %.s, $(OBJ_DIR)/%.o, $(SRC_FILES_S))
OBJ_FILES_ASM := $(patsubst %.asm, $(OBJ_DIR)/%.o, $(SRC_FILES_ASM))

all:
	# python update_version.py
	make AthenX.bin
	(cd userspace && make)
# Compile C files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(GCCPARAMS) -c -o $@ $<

# Assemble assembly files
$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASPARAMS) -o $@ $<

# Assemble NASM files
$(OBJ_DIR)/%.o: %.asm
	@mkdir -p $(@D)
	$(ASM) -f elf32 -o $@ $<

# Link object files into binary
AthenX.bin: $(OBJ_FILES_C) $(OBJ_FILES_S) $(OBJ_FILES_ASM)

	ld $(LDPARAMS) -o $@ $^
	mkdir -p isodir/boot/grub
	cp AthenX.bin isodir/boot/AthenX.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o AthenX.iso isodir


# Run the OS in QEMU
run: AthenX.bin
	
	bash ./scripts/boot32.sh
	qemu-system-i386 -drive file=AthenX.img,format=raw \
	-drive id=disk,file=ahci.img,format=raw,if=none \
	-device ahci,id=ahci \
	-device ide-hd,drive=disk,bus=ahci.0 \
	-m 4G \
	-serial stdio \
	-trace ahci_* 



	bash ./scripts/athenxHost.sh


runt:
	qemu-system-i386 -drive file=AthenX.img,format=raw \
  -m 4G -serial file:out.log -serial stdio 


libc:
	(cd libc  && make )

user:
	(cd user && make)

# Clean up generated files
clean:
	rm -rf $(OUT_DIR)/*.bin $(OUT_DIR)/*.map $(OUT_DIR)/*.img $(OBJ_DIR)/* AthenX.iso AthenX.bin libc/libc.a

.PHONY: user libc