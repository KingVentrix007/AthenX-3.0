#ifndef __EXE__H
#define __EXE__H
#include <stdint.h>
#include "elf.h"
#include <stddef.h>
#define EXECUTABLE_BASE_ADDRESS 0x90000
typedef uintptr_t addr;
typedef enum executable_type
{
  ELF32_EXECUTABLE,
  FLAT_EXECUTABLE
} executable_type;

typedef struct executable
{
  addr entry_point;
  addr program_break;
  executable_type type;
} executable;
typedef struct {
    char name[256]; // Adjust the size as needed
} ProgramEntry;
#define MAX_PROGRAMS 100
void load_elf_file(const char* filename, int argc, char **argv);
void load_exe_file(const char* filename, uint8_t* stack);
int find_programs(const char *path);
int get_program_list(char *list[MAX_PROGRAMS]);
#endif