#ifndef __DEBUG__H
#define __DEBUG__H
#include "kernel.h"
#include <stddef.h> 
#define MAX_LINE_LENGTH 256
#define MAX_FUNCTION_NAME 128
#define MAX_FUNCTION_NAME 128
#define MAX_LINE_LENGTH 256
#define MAX_PARAMS 10
#define MAX_FRAMES 100
struct stackframe {
    struct stackframe* ebp;
    uint32_t eip;
};
typedef struct {
    char function_name[MAX_FUNCTION_NAME];
    char file_path[MAX_LINE_LENGTH];
    unsigned int func_address;
    char parms[1024];
} FunctionInfo;
void print_kernel_memory_map(KERNEL_MEMORY_MAP *memory_map);
// int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES],int error_code);
int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES], int error_code);
extern char *debug_map;
extern FunctionInfo found_functions[MAX_FRAMES];
const FunctionInfo* find_function(const char *buffer, size_t buffer_size, unsigned int address);
extern int num_found_functions;
#endif