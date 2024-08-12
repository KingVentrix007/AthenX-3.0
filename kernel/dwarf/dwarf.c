#include "elf.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fat_filelib.h"
#include "debug.h"
#include "dwarf.h"

void parse_debug_info(FILE *file, Elf32_Shdr *shdrs, int shnum, const char *shstrtab_data);
void parse_debug_abbrev(FILE *file, Elf32_Shdr *shdr, uint32_t offset, uint32_t size);
void parse_debug_abbrev_offset(FILE *file, Elf32_Shdr *shdrs, int shnum, const char *shstrtab_data, uint32_t offset);


size_t decode_uleb128(const uint8_t *ptr, uint32_t *value) {
    const uint8_t *start = ptr;
    uint32_t result = 0;
    int shift = 0;
    while (1) {
        uint8_t byte = *ptr++;
        result |= (byte & 0x7f) << shift;
        if ((byte & 0x80) == 0) break;
        shift += 7;
    }
    *value = result;
    return ptr - start;
}

size_t decode_sleb128(const uint8_t *ptr, int32_t *value) {
    const uint8_t *start = ptr;
    int32_t result = 0;
    int shift = 0;
    uint8_t byte;
    do {
        byte = *ptr++;
        result |= (byte & 0x7f) << shift;
        shift += 7;
    } while (byte & 0x80);

    if (shift < 32 && (byte & 0x40)) {
        result |= -(1 << shift);
    }
    *value = result;
    return ptr - start;
}


void find_section_for_address(const char *elf_file_path) {
    FILE *file = fl_fopen(elf_file_path, "rb");
    if (!file) {
        printf("Failed to open ELF file\n");
        return;
    }

    // Read the ELF header
    Elf32_Ehdr ehdr;
    fseek(file, 0, SEEK_SET);
    if (fread(&ehdr, sizeof(ehdr), 1, file) != sizeof(ehdr)) {
        printf("Failed to read ELF header\n");
        fclose(file);
        return;
    }

    // Check if the file is an ELF file
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Not an ELF file\n");
        fclose(file);
        return;
    }

    // Read section headers
    fseek(file, ehdr.e_shoff, SEEK_SET);
    Elf32_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!shdrs) {
        printf("Failed to allocate memory for section headers\n");
        fclose(file);
        return;
    }
    if (fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, file) != ehdr.e_shnum*ehdr.e_shentsize) {
        printf("Failed to read section headers\n");
        free(shdrs);
        fclose(file);
        return;
    }

    // Read the section header string table
    Elf32_Shdr shstrtab = shdrs[ehdr.e_shstrndx];
    char *shstrtab_data = malloc(shstrtab.sh_size);
    if (!shstrtab_data) {
        printf("Failed to allocate memory for section header string table\n");
        free(shdrs);
        fclose(file);
        return;
    }
    fseek(file, shstrtab.sh_offset, SEEK_SET);
    if (fread(shstrtab_data, shstrtab.sh_size, 1, file) != shstrtab.sh_size) {
        printf("Failed to read section header string table\n");
        free(shstrtab_data);
        free(shdrs);
        fclose(file);
        return;
    }

    // Find and parse the .debug_info and .debug_abbrev sections
    for (int i = 0; i < ehdr.e_shnum; ++i) {
        const char *section_name = &shstrtab_data[shdrs[i].sh_name];
        if (strcmp(section_name, ".debug_info") == 0) {
            parse_debug_info(file, shdrs, ehdr.e_shnum, shstrtab_data);
        }
    }

    free(shstrtab_data);
    free(shdrs);
    fclose(file);
}

void parse_debug_info(FILE *file, Elf32_Shdr *shdrs, int shnum, const char *shstrtab_data) {
    for (int i = 0; i < shnum; ++i) {
        const char *section_name = &shstrtab_data[shdrs[i].sh_name];
        if (strcmp(section_name, ".debug_info") == 0) {
            fseek(file, shdrs[i].sh_offset, SEEK_SET);
            uint8_t *debug_info_data = malloc(shdrs[i].sh_size);
            if (!debug_info_data) {
                printf("Failed to allocate memory for .debug_info section\n");
                return;
            }
            if (fread(debug_info_data, shdrs[i].sh_size, 1, file) != shdrs[i].sh_size) {
                printf("Failed to read .debug_info section\n");
                free(debug_info_data);
                return;
            }

            uint8_t *ptr = debug_info_data;
            while (ptr < debug_info_data + shdrs[i].sh_size) {
                DWARF_CU_Header *cu_header = (DWARF_CU_Header *)ptr;
                uint32_t cu_length = cu_header->unit_length;
                printf("CU Length: 0x%x\n", cu_length);
                printf("CU Version: %u\n", cu_header->version);
                printf("CU Abbrev Offset: 0x%x\n", cu_header->abbrev_offset);
                printf("CU Address Size: %u\n", cu_header->address_size);

                for (int j = 0; j < shnum; ++j) {
                    const char *abbrev_section_name = &shstrtab_data[shdrs[j].sh_name];
                    if (strcmp(abbrev_section_name, ".debug_abbrev") == 0) {
                        parse_debug_abbrev(file, &shdrs[j], cu_header->abbrev_offset, shdrs[j].sh_size);
                        break;
                    }
                }

                ptr += cu_length + sizeof(uint32_t);
            }

            free(debug_info_data);
        }
    }
}




void parse_debug_abbrev_offset(FILE *file, Elf32_Shdr *shdrs, int shnum, const char *shstrtab_data,uint32_t offset)
{

}











void parse_debug_abbrev(FILE *file, Elf32_Shdr *shdr, uint32_t offset, uint32_t size) {
    fseek(file, shdr->sh_offset + offset, SEEK_SET);
    uint8_t *debug_abbrev_data = malloc(size - offset);
    if (!debug_abbrev_data) {
        printf("Failed to allocate memory for .debug_abbrev section\n");
        return;
    }
    if (fread(debug_abbrev_data, size - offset, 1, file) != size - offset) {
        printf("Failed to read .debug_abbrev section\n");
        free(debug_abbrev_data);
        return;
    }

    uint8_t *ptr = debug_abbrev_data;
    while (ptr < debug_abbrev_data + (size - offset)) {
        uint32_t abbrev_code = 0;
        ptr += decode_uleb128(ptr, &abbrev_code);
        if (abbrev_code == 0) break; // Abbreviation code 0 marks the end of the set

        uint32_t tag = 0;
        ptr += decode_uleb128(ptr, &tag);
        uint8_t has_children = *ptr++;

        printf("Abbrev Code: %u\n", abbrev_code);
        printf("Tag: %u\n", tag);
        printf("Has Children: %u\n", has_children);

        while (1) {
            uint32_t name = 0;
            uint32_t form = 0;
            ptr += decode_uleb128(ptr, &name);
            ptr += decode_uleb128(ptr, &form);
            if (name == 0 && form == 0) break;

            printf("Name: %u, Form: %u\n", name, form);
        }
    }

    free(debug_abbrev_data);
}
