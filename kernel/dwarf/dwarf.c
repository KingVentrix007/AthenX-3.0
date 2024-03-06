#include "elf.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fat_filelib.h"
void dwarf_function(uintptr_t address, char* function_name) {
    printf("Looking for function\n");
    // if (!function_name) {
    //     fprintf(stderr, "Function name not provided\n");
    //     return;
    // }

    // // Find and print the function name
    // printf("function name\n");
    // find_function_name(function_name, address);
}
// void print_elf_sections(const char* file_path) {
//     // Open the ELF file
//     int fd = fopen(file_path, "r");
//     if (fd == -1) {
//         perror("Failed to open file");
//         return;
//     }

//     // Read ELF header
//     Elf32_Ehdr elf_header;
//     if (fread(fd, &elf_header, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
//         perror("Failed to read ELF header");
//         close(fd);
//         return;
//     }

//     // Check if it's a valid ELF file
//     if (elf_header.e_ident[EI_MAG0] != ELFMAG0 || 
//         elf_header.e_ident[EI_MAG1] != ELFMAG1 || 
//         elf_header.e_ident[EI_MAG2] != ELFMAG2 || 
//         elf_header.e_ident[EI_MAG3] != ELFMAG3) {
//         printf("Not a valid ELF file\n");
//         close(fd);
//         return;
//     }

//     // Seek to the section header offset
//     fseek(fd, elf_header.e_shoff, SEEK_SET);

//     // Read section headers and print their names
//     printf("Sections:\n");
//     for (int i = 0; i < elf_header.e_shnum; ++i) {
//         Elf32_Shdr section_header;
//         if (read(fd, &section_header, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)) {
//             perror("Failed to read section header");
//             close(fd);
//             return;
//         }

//         // Print section name
//         printf("%d: %s\n", i, (char*)((uintptr_t)elf_header.e_shstrndx + section_header.sh_name));
//     }

//     // Close the file
//     close(fd);
// }