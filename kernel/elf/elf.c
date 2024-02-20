#include "elf.h"
#include "fat_filelib.h"
#include "printf.h"
// #include "mem.h"

char* read_txt_section(const char* file_path) {
    FL_FILE* file = fl_fopen(file_path, "rb");

    if (!file) {
        //TODO implement errno, perror, and other error fuctions
        printf("Error opening file");
        return NULL;
    }

    // Read the ELF header
    Elf64_Ehdr elf_header;
    fl_fread(&elf_header, sizeof(Elf64_Ehdr), 1, file);

    // Check if it's a valid ELF file
    if (elf_header.e_ident[EI_MAG0] != ELFMAG0 || elf_header.e_ident[EI_MAG1] != ELFMAG1 ||
        elf_header.e_ident[EI_MAG2] != ELFMAG2 || elf_header.e_ident[EI_MAG3] != ELFMAG3) {
        printf("Not a valid ELF file\n");
        fl_fclose(file);
        return NULL;
    }

    // Find the section header string table
    fl_fseek(file, elf_header.e_shoff + elf_header.e_shentsize * elf_header.e_shstrndx, SEEK_SET);
    Elf64_Shdr shstrtab_header;
    fl_fread(&shstrtab_header, sizeof(Elf64_Shdr), 1, file);

    // Read the section names
    char* section_names = kmalloc(shstrtab_header.sh_size);
    fl_fseek(file, shstrtab_header.sh_offset, SEEK_SET);
    fl_fread(section_names, shstrtab_header.sh_size, 1, file);

    // Find the .txt section
    Elf64_Shdr txt_section_header;
    int txt_section_found = 0;

    for (int i = 0; i < elf_header.e_shnum; i++) {
        fl_fseek(file, elf_header.e_shoff + i * elf_header.e_shentsize, SEEK_SET);
        fl_fread(&txt_section_header, sizeof(Elf64_Shdr), 1, file);

        if (txt_section_header.sh_type == SHT_PROGBITS && strcmp(section_names + txt_section_header.sh_name, ".txt") == 0) {
            txt_section_found = 1;
            break;
        }
    }

    // Read the contents of the .txt section
    char* txt_section_contents = NULL;

    if (txt_section_found) {
        txt_section_contents = kmalloc(txt_section_header.sh_size);
        fl_fseek(file, txt_section_header.sh_offset, SEEK_SET);
        fl_fread(txt_section_contents, txt_section_header.sh_size, 1, file);
    } else {
        printf("No .txt section found\n");
    }

    // Clean up and close the file
    kfree(section_names);
    fl_fclose(file);

    return txt_section_contents;
}