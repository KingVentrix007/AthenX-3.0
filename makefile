CC = i686-elf-gcc
AS = as
ASM = nasm
CONFIG = ./config
GCCPARAMS =  -O0 -I./include -fno-omit-frame-pointer -nostdlib -fno-pic -fno-builtin -fno-exceptions -ffreestanding -fno-leading-underscore -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long

ASPARAMS = --32
LDPARAMS = -m elf_i386 -T linker.ld -nostdlib 

OBJ_DIR = obj
SRC_DIRS = arch kernel drivers libk asm
OBJ_DIRS = $(addprefix $(OBJ_DIR)/,$(SRC_DIRS))

$(shell mkdir -p $(OBJ_DIRS))

SRC_FILES_C := $(shell find $(SRC_DIRS) -type f -name '*.c')
SRC_FILES_S := $(shell find $(SRC_DIRS) -type f -name '*.s')
SRC_FILES_ASM := $(shell find $(SRC_DIRS) -type f -name '*.asm')

OBJ_FILES_C := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC_FILES_C))
OBJ_FILES_S := $(patsubst %.s, $(OBJ_DIR)/%.o, $(SRC_FILES_S))
OBJ_FILES_ASM := $(patsubst %.asm, $(OBJ_DIR)/%.o, $(SRC_FILES_ASM))

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(GCCPARAMS) -c -o $@ $<


$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASPARAMS) -o $@ $<

$(OBJ_DIR)/%.o: %.asm
	@mkdir -p $(@D)
	$(ASM) -f elf32 -o $@ $<

AthenX.bin: $(OBJ_FILES_C) $(OBJ_FILES_S) $(OBJ_FILES_ASM)
	ld $(LDPARAMS) -o $@ $^
	mkdir -p isodir/boot/grub
	cp AthenX.bin isodir/boot/AthenX.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o AthenX.iso isodir
#	mv AthenX.bin out/
run: AthenX.bin
	bash ./scripts/boot32.sh
	qemu-system-i386 AthenX.img -m 4G
runt:
	qemu-system-i386 AthenX.img -m 4G
clean:
	rm -rf $(OUT_DIR)/*.bin $(OUT_DIR)/*.map $(OUT_DIR)/*.img $(OBJ_DIR)/* AthenX.iso AthenX.bin
