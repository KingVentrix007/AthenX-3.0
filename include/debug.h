#ifndef __DEBUG__H
#define __DEBUG__H
#include "kernel.h"
#include <stddef.h>
#include <stdbool.h>
#include "dwarf.h"
#define MAX_LINE_LENGTH 256
#define MAX_FUNCTION_NAME 128
#define MAX_PARAMS 10
#define MAX_FRAMES 100
struct stackframe {
    struct stackframe* ebp;
    uint32_t eip;
};
typedef struct {
    char function_name[MAX_FUNCTION_NAME];
    char file_path[MAX_LINE_LENGTH];
    int line_num;
    unsigned int func_address;
    char parms[1024];
    uint32_t eip;
    bool is_error;
}FunctionInfo;
void print_kernel_memory_map(KERNEL_MEMORY_MAP *memory_map);
// int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES],int error_code);
int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES], int error_code,uint32_t eip);
extern char *debug_map;
extern FunctionInfo found_functions[MAX_FRAMES];
const FunctionInfo* find_function(const char *buffer, size_t buffer_size, unsigned int address);
extern int num_found_functions;
const FunctionInfo* find_function_with_biggest_address(const char *buffer, size_t buffer_size);
const FunctionInfo* find_function_with_smallest_address(const char *buffer, size_t buffer_size);
void initialize_decoder_memory();

#endif