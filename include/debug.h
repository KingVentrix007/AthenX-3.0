#ifndef __DEBUG__H
#define __DEBUG__H
#include "kernel.h"

#define MAX_LINE_LENGTH 256
#define MAX_FUNCTION_NAME 128

struct stackframe {
    struct stackframe* ebp;
    uint32_t eip;
};
void print_kernel_memory_map(KERNEL_MEMORY_MAP *memory_map);
void walk_stack(struct stackframe *stk);
extern char *debug_map;
#endif