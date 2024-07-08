#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "elf.h"
#include "file.h"
#include "fat_filelib.h"

#define ELFCLASS32 1
#define ELFCLASS64 2

#define EI_CLASS 4

#define SHT_PROGBITS 1

typedef struct __attribute__((packed)) {
    uint32_t length;
    uint16_t version;
    uint32_t header_length;
    uint8_t min_instruction_length;
    uint8_t default_is_stmt;
    int8_t line_base;
    uint8_t line_range;
    uint8_t opcode_base;
    uint8_t std_opcode_lengths[12];
} DebugLineHeader;

void parse_debug_line_section(FILE *file, Elf32_Shdr *section_header) {
    // Read the entire .debug_line section
    fseek(file, section_header->sh_offset, SEEK_SET);
    uint32_t section_end = section_header->sh_offset + section_header->sh_size;

    while (ftell(file) < section_end) {
        DebugLineHeader header;
        fread(&header, sizeof(header), 1, file);

        // Process the Debug Line Header
        printf("Length: %u\n", header.length);
        printf("Version: %u\n", header.version);
        printf("Header Length: %u\n", header.header_length);
        printf("Minimum Instruction Length: %u\n", header.min_instruction_length);
        printf("Default Is Statement: %u\n", header.default_is_stmt);
        printf("Line Base: %d\n", header.line_base);
        printf("Line Range: %u\n", header.line_range);
        printf("Opcode Base: %u\n", header.opcode_base);

        // Skip over standard opcode lengths (not needed for basic parsing)
        fseek(file, header.header_length - sizeof(DebugLineHeader), SEEK_CUR);

        // Read and process line number program instructions
        uint8_t opcode;
        while (ftell(file) < section_header->sh_offset + section_header->sh_size) {
            fread(&opcode, sizeof(uint8_t), 1, file);
            if (opcode == 0) {
                // End of the line number program
                break;
            } else if (opcode < header.opcode_base) {
                // Special opcode
                // Adjust address and line
                uint8_t adjusted_opcode = opcode;
                uint8_t line_increment = adjusted_opcode % header.line_range;
                int address_increment = adjusted_opcode / header.line_range;
                fseek(file, address_increment, SEEK_CUR);
            } else {
                // Standard opcode
                // Adjust the address
                fseek(file, opcode - header.opcode_base, SEEK_CUR);
            }
        }
    }
}

void parse_elf32(const char *binary_path) {
    FILE *file = fl_fopen(binary_path, "rb");
    if (!file) {
        printf("Failed to open binary %s\n", binary_path);
        return;
    }

    Elf32_Ehdr ehdr;
    fread(&ehdr, 1, sizeof(ehdr), file);

    // Check ELF magic number
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Not an ELF file\n");
        fclose(file);
        return;
    }

    // Check ELF class
    if (ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
        printf("Not a 32-bit ELF file\n");
        fclose(file);
        return;
    }

    Elf32_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!shdrs) {
        printf("Memory allocation failed\n");
        fclose(file);
        return;
    }

    fseek(file, ehdr.e_shoff, SEEK_SET);
    fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, file);

    char *strtab = NULL;
    for (int i = 0; i < ehdr.e_shnum; ++i) {
        if (shdrs[i].sh_type == SHT_PROGBITS) {
            char section_name[20];
            fseek(file, shdrs[ehdr.e_shstrndx].sh_offset + shdrs[i].sh_name, SEEK_SET);
            fread(section_name, 1, sizeof(section_name), file);
            if (strcmp(section_name, ".debug_line") == 0) {
                printf("Section .debug_line found at offset 0x%x with size 0x%x\n", 
                       shdrs[i].sh_offset, shdrs[i].sh_size);

                // Load and process the .debug_line section
                parse_debug_line_section(file, &shdrs[i]);
            }
        } else if (shdrs[i].sh_type == SHT_STRTAB) {
            if (i == ehdr.e_shstrndx) {
                strtab = malloc(shdrs[i].sh_size);
                if (!strtab) {
                    printf("Memory allocation failed\n");
                    fclose(file);
                    free(shdrs);
                    return;
                }
                fseek(file, shdrs[i].sh_offset, SEEK_SET);
                fread(strtab, 1, shdrs[i].sh_size, file);
            }
        }
    }

    free(shdrs);
    free(strtab);
    fclose(file);
}
