#include "kernel.h"
#include "debug.h"
#include <stdarg.h>
#include "stdbool.h"
#include <stddef.h>
#include "stdlib.h"
#include "isr.h"
#include "scanf.h"
#include "printf.h"
#include "string.h"
#include "elf.h"
#include "stdio.h"
#include "fat_filelib.h"
#include "io_ports.h"


#include <stdio.h>
#include <stdint.h>

int print_stack_frame(uintptr_t *base, size_t size,FunctionInfo functions[MAX_FRAMES]) {
    uint8_t *current = (uint8_t *)base;
    uint8_t *end = current + size;

    while (current < end) {
        uintptr_t value = *(uintptr_t *)current;
        
        uint8_t opcode = current[0];
        size_t instruction_length = 1;

        switch (opcode) {
        case 0x55:
            instruction_length = 1;
            printf("    push   %%ebp\n");
            break;
        case 0xc3:
            instruction_length = 1;
            printf("    ret\n");
            return;
        case 0x89:
            if (current[1] == 0xe5) {
                instruction_length = 2;
                printf("    mov    %%esp, %%ebp\n");
            } else if (current[1] == 0x45 && current[2] == 0xfc) {
                instruction_length = 3;
                printf("    mov    %%eax, -0x4(%%ebp)\n");
            } else {
                printf("    Unhandled mov\n");
            }
            break;
        case 0x83:
            if (current[1] == 0xec) {
                instruction_length = 3;
                printf("    sub    $0x%x, %%esp\n", current[2]);
            } else if (current[1] == 0xc4) {
                instruction_length = 3;
                printf("    add    $0x%x, %%esp\n", current[2]);
            } else {
                printf("    Unhandled 0x83 opcode\n");
            }
            break;
        case 0xb8:
            instruction_length = 5;
            printf("    mov    $0x%x, %%eax\n", *(int32_t *)(current + 1));
            break;
        case 0xb9:
            instruction_length = 5;
            printf("    mov    $0x%x, %%ecx\n", *(int32_t *)(current + 1));
            break;
        case 0x99:
            instruction_length = 1;
            printf("    cltd\n");
            break;
        case 0xf7:
            if (current[1] == 0xf9) {
                instruction_length = 2;
                printf("    idiv   %%ecx\n");
            } else {
                printf("    Unhandled 0xf7 opcode\n");
            }
            break;
        case 0x90:
            instruction_length = 1;
            printf("    nop\n");
            break;
        case 0xc9:
            instruction_length = 1;
            printf("    leave\n");
            break;
        case 0xe8:
            {
                int32_t offset = *(int32_t *)(current + 1);
                uintptr_t target_address = (uintptr_t)current + 5 + offset;
                instruction_length = 5;
                printf("    call   0x%08lx\n", target_address);
            }
            break;
        case 0xc7:
            if (current[1] == 0x45 && current[2] == 0xf4) {
                instruction_length = 7;
                int32_t imm = *(int32_t *)(current + 3);
                printf("    movl   $0x%x(%s), -0xc(%%ebp)\n", imm,imm);
            } else {
                printf("    Unhandled 0xc7 opcode\n");
            }
            break;
        case 0xff:
            if (current[1] == 0x75) {
                instruction_length = 3;
                printf("    push   -0xc(%%ebp)\n");
            } else {
                printf("    Unhandled 0xff opcode\n");
            }
            break;
        default:
            printf("    Address 0x%08lx: 0x%08lx - ", (uintptr_t)current, value);

            printf("    Unhandled opcode [0x%x]\n", opcode);
            break;
        }
        
        current += instruction_length;
    }
    
    return 0;
}
