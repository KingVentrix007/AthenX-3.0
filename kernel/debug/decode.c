#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "debug.h"
#include "printf.h"
#include "stdlib.h"
// Define the maximum length for function names
#define MAX_FUNCTION_NAME 64
#define MAX_FRAMES 128

// Structure to hold function information

// Define the structure to hold register states
typedef struct {
    int32_t eax;
    int32_t ebx;
    int32_t ecx;
    int32_t edx;
    int32_t esi;
    int32_t edi;
    int32_t esp;
    int32_t ebp;
} RegisterState;

// Function prototypes
void print_register_state(RegisterState *regs);

// Function to print register state
void print_register_state(RegisterState *regs) {
    printf("    Register State:\n");
    printf("        EAX: 0x%08x\n", regs->eax);
    printf("        EBX: 0x%08x\n", regs->ebx);
    printf("        ECX: 0x%08x\n", regs->ecx);
    printf("        EDX: 0x%08x\n", regs->edx);
    printf("        ESI: 0x%08x\n", regs->esi);
    printf("        EDI: 0x%08x\n", regs->edi);
    printf("        ESP: 0x%08x\n", regs->esp);
    printf("        EBP: 0x%08x\n", regs->ebp);
}
int is_movl_string(uintptr_t addr)
{
    char *str = (const char *) addr;
    if(*str == '\0')
    {
        return -1;
    }
    else
    {
        while(*str != '\0')
        {
            if(*str < 32)
            {
                return -1;
            }
            if(*str > 126)
            {
                return -1;
            }
            str++;
        }
        return 1;
    }
}
int get_movl_type(int32_t imm)
{
    uintptr_t addr = (uintptr_t)imm;
    if(is_movl_string(addr) == 1)
    {
        return 1;
    }
    
    return -1;
}
const char* get_register_name(uint8_t reg) {
    switch (reg) {
        case 0x00: return "eax";
        case 0x01: return "ecx";
        case 0x02: return "edx";
        case 0x03: return "ebx";
        case 0x04: return "esp";
        case 0x05: return "ebp";
        case 0x06: return "esi";
        case 0x07: return "edi";
        default: return "unknown";
    }
}

// To simulate a stack and use registers for your disassembler, you can indeed allocate a region of memory to act as the stack and then set one of the registers (like esp or ebp) to point to this allocated region. Here's a step-by-step approach to achieve this:

// 1. Allocate Memory for the Stack
// Use malloc to allocate a region of memory to simulate your stack:

// c
// Copy code
#define STACK_SIZE 4096
int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES], int error_code) {
    void *stack = malloc(STACK_SIZE);
    uint8_t *current = (uint8_t *)base;
    uint8_t *end = current + size;
    RegisterState regs = {0};
    regs.esp = (uintptr_t)stack + STACK_SIZE; 
    char func[MAX_FUNCTION_NAME] = "unkown";
    for (size_t i = 0; i < num_found_functions; i++)
    {
        FunctionInfo *info = &functions[i];
        if(info->func_address == base)
        {
            memset(func,0,MAX_FUNCTION_NAME);
            strcpy(func,info->function_name);
        }
    }
    printf("0x%x <%s>\n",base,func);
    while (current < end) {
        uintptr_t value = *(uintptr_t *)current;
        uint8_t opcode = current[0];
        size_t instruction_length = 1;
        printf("%x",(uintptr_t)current);
        switch (opcode) {
        case 0x55:
            instruction_length = 1;
            printf("    push   %%ebp\n");
            regs.esp -= 4;  // Adjust stack pointer
            *(uintptr_t *)regs.esp = regs.ebp;  // Save %ebp to stack
            break;
        case 0x50:
            instruction_length = 1;
            regs.esp -= 4;  // Adjust stack pointer
            *(uintptr_t *)regs.esp = regs.eax;  // Save %eax to stack
            printf("    push   %%eax\n");
            break;
        case 0xc3:
            instruction_length = 1;
            printf("    ret\n");
            return 0; // End of function
        case 0x89: {
                uint8_t mod = (current[1] >> 6) & 0x03;
                uint8_t reg = (current[1] >> 3) & 0x07;
                uint8_t rm = current[1] & 0x07;

                switch (mod) {
                    case 0x00: // No displacement
                        if (rm == 0x05) { // Direct address
                            instruction_length = 6;
                            uintptr_t addr = *(uintptr_t *)(current + 2);
                            printf("    mov    %%eax, 0x%lx\n", addr);
                        } else {
                            printf("    Unhandled 0x89 opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                        }
                        break;
                    case 0x01: // 8-bit displacement
                    case 0x02: // 32-bit displacement
                        instruction_length = 3 + (mod == 0x02 ? 4 : 1);
                        int32_t displacement = (mod == 0x01) ? (int8_t)current[2] : *(int32_t *)(current + 2);
                        if (rm == 0x05) {
                            printf("    mov    %%eax, %d(%%ebp)\n", displacement);
                        } else {
                            printf("    Unhandled 0x89 opcode with displacement, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                        }
                        instruction_length = 3 ;
                        break;
                    case 0x03: // Register mode
                        instruction_length = 2;
                        printf("    mov    %%%s, %%%s\n", get_register_name(rm), get_register_name(reg));
                        break;
                    default:
                        printf("    Unhandled 0x89 opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                        break;
                }
                break;
            }

                
        case 0x8b: {
                uint8_t mod = (current[1] >> 6) & 0x03;
                uint8_t reg = (current[1] >> 3) & 0x07;
                uint8_t rm = current[1] & 0x07;
                size_t disp_size = (mod == 0x01) ? 1 : (mod == 0x02) ? 4 : 0;

                switch (mod) {
                    case 0x00:
                        if (rm == 0x05) { // Direct address
                            instruction_length = 6;
                            uintptr_t addr = *(uintptr_t *)(current + 2);
                            printf("    mov    %%eax, 0x%lx\n", addr);
                        } else {
                            printf("    Unhandled 0x8b opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                        }
                        break;
                    case 0x01:
                    case 0x02:
                        instruction_length = 2 + disp_size;
                        int32_t displacement = (mod == 0x01) ? (int8_t)current[2] : *(int32_t *)(current + 2);
                        printf("    mov    %%%s, %d(%%ebp)\n", get_register_name(reg), displacement);
                        
                        break;
                    case 0x03:
                        instruction_length = 2;
                        printf("    mov    %%%s, %%%s\n", get_register_name(reg), get_register_name(rm));
                        break;
                    default:
                        printf("    Unhandled 0x8b opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                        break;
                }
                break;
            }

        case 0x83:
            if (current[1] == 0xec) {
                instruction_length = 3;
                printf("    sub    $0x%x, %%esp\n", current[2]);
                regs.esp -= current[2];
            } else if (current[1] == 0xc4) {
                instruction_length = 3;
                printf("    add    $0x%x, %%esp\n", current[2]);
                regs.esp += current[2];
            }else if (current[1] ==0xe0)
            {
                instruction_length = 3;
                printf("    add    $0x%x, %%eax\n", current[2]);
            }
             
            else if (current[1] == 0x7d && current[2] == 0xf4) {
                instruction_length = 4;
                printf("    cmpl   $0x%x, -0xc(%%ebp)\n", current[3]);
            }else if (current[1] == 0x7d && current[2] == 0xf0)
            {
                instruction_length = 4;
                printf("    cmpl   $0x%x, -0x10(%%ebp)\n", current[3]);
            }
            
            else if (current[1] == 0xc8)
            {
                printf("    or     $0x%x, %%eax\n", current[2]);
                instruction_length = 3;

            }
             
            else {
                printf("    Unhandled 0x83 opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0xb8:
                instruction_length = 5;
                printf("    mov    $0x%x, %%eax\n", *(int32_t *)(current + 1));
                regs.eax = *(int32_t *)(current + 1);
                break;
        case 0xb9:
                instruction_length = 5;
                printf("    mov    $0x%x, %%ecx\n", *(int32_t *)(current + 1));
                regs.ecx = *(int32_t *)(current + 1);
                break;

        case 0x99:
            instruction_length = 1;
            printf("    cltd\n");
            break;
        case 0xf7:
            if (current[1] == 0xf9) {
                instruction_length = 2;
                
                printf("    idiv   %%ecx ");
                printf("%d/%d",regs.eax,regs.edx);
                if (error_code == 0) {
                    printf("<-- Possible source of error");
                }
                printf("\n");
            }else if ((current[1] & 0xf8) == 0x78) {
        // Handle cases like f7 7d f8 for idivl -0x8(%ebp)
        if ((current[1] & 0xc0) == 0x40) { // ModR/M byte with 8-bit displacement
            int8_t displacement = (int8_t)current[2];
            printf("    idivl  %d(%%ebp)", displacement);
            printf("%d/%d",regs.eax,regs.edx);
            if (error_code == 0) {
                printf("<-- Possible source of error");
            }
            printf("\n");
            instruction_length = 3;
        } else {
            printf("    Unhandled 0xf7 7x opcode, Second byte [0x%x]\n", current[1]);
        }
    } else {
        printf("    Unhandled 0xf7 opcode, Second byte [0x%x]\n", current[1]);
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
                char func_name[MAX_FUNCTION_NAME] = "<unknown>";
                for (size_t i = 0; i < num_found_functions; i++) {
                    FunctionInfo *func = &functions[i];
                    if (func->func_address == target_address) {
                        memset(func_name, 0, MAX_FUNCTION_NAME);
                        strcpy(func_name, func->function_name);
                    }
                }
                if(strcmp(func_name,"<unknown>") == 0)
                {
                    FunctionInfo *func = find_function(debug_map, strlen(debug_map),target_address);
                    if(func)
                    {
                        memset(func_name, 0, MAX_FUNCTION_NAME);
                        strcpy(func_name, func->function_name);
                    }
                }
                printf("    call   0x%08lx <%s>\n", target_address, func_name);
            }
            break;
        case 0xc7: {
    uint8_t mod = (current[1] >> 6) & 0x03;
    uint8_t reg = (current[1] >> 3) & 0x07;
    uint8_t rm = current[1] & 0x07;

    switch (mod) {
        case 0x00: // No displacement
            if (rm == 0x05) { // Direct address
                instruction_length = 10;
                uintptr_t addr = *(uintptr_t *)(current + 2);
                int32_t imm = *(int32_t *)(current + 6);
                printf("    movl   $0x%x, 0x%lx\n", imm, addr);
            } else {
                printf("    Unhandled 0xc7 opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
            }
            break;
        case 0x01: // 8-bit displacement
        case 0x02: // 32-bit displacement
            {
                int32_t displacement;
                if (mod == 0x01) {
                    instruction_length = 7;
                    displacement = (int8_t)current[2];
                } else { // mod == 0x02
                    instruction_length = 7;
                    displacement = *(int32_t *)(current + 2);
                }
                int32_t imm = *(int32_t *)(current + instruction_length - 4);
                if (rm == 0x05) {
                    int data_type = get_movl_type(imm);
                    if(data_type == 1)
                    {
                        printf("    movl   $0x%x(%.5s), %d(%%ebp)\n", imm,imm, displacement);

                    }
                    else
                    {
                        printf("    movl   $0x%x(%d), %d(%%ebp)\n", imm,imm, displacement);
                    }
                    regs.ebp = imm;
                    
                } else {
                    printf("    Unhandled 0xc7 opcode with displacement, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                }
            }
            break;
        case 0x03: // Register mode
            printf("    movl   %%%s, %%%s\n", get_register_name(rm), get_register_name(reg));
            instruction_length = 2;
            break;
        default:
            printf("    Unhandled 0xc7 opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
            break;
    }
    break;
}
        case 0xff:
            if (current[1] == 0x75) {
                instruction_length = 3;
                printf("    push   -0xc(%%ebp)\n");
            } else {
                printf("    Unhandled 0xff opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0x53:
            instruction_length = 1;
            printf("    push   %%ebx\n");
            break;
        case 0x68:
            {
                instruction_length = 5;
                int32_t imm = *(int32_t *)(current + 1);
                printf("    push   $0x%x\n", imm);
            }
            break;
        case 0x6a:
            {
                instruction_length = 2;
                int8_t imm = *(int8_t *)(current + 1);
                printf("    push   $0x%x\n", imm);
            }
            break;
        case 0xa3:
            {
                instruction_length = 5;
                uintptr_t addr = *(uintptr_t *)(current + 1);
                printf("    mov    %%eax, 0x%lx\n", addr);
            }
            break;
        case 0xa1:
            {
                instruction_length = 5;
                uintptr_t addr = *(uintptr_t *)(current + 1);
                printf("    mov    0x%lx, %%eax\n", addr);
            }
            break;
        case 0x85:
            if (current[1] == 0xc0) {
                instruction_length = 2;
                printf("    test   %%eax, %%eax\n");
            } else {
                printf("    Unhandled 0x85 opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0x75:
            {
                instruction_length = 2;
                int8_t offset = *(int8_t *)(current + 1);
                uintptr_t target_address = (uintptr_t)current + 2 + offset;
                printf("    jne    0x%lx\n", target_address);
            }
            break;
        case 0x74:
            {
                instruction_length = 2;
                int8_t offset = *(int8_t *)(current + 1);
                uintptr_t target_address = (uintptr_t)current + 2 + offset;
                printf("    je     0x%lx\n", target_address);
            }
            break;
        case 0x0f:
            if (current[1] == 0xb6 && current[2] == 0x00) {
                instruction_length = 3;
                printf("    movzbl (%s), %s\n", "%%eax", "%%eax");
            } else {
                printf("    Unhandled 0x0f opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0x84:
            if (current[1] == 0xc0) {
                instruction_length = 2;
                printf("    test   %s, %s\n", "%%al", "%%al");
            } else {
                printf("    Unhandled 0x84 opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0xe9:
            instruction_length+=4;
            uint8_t *next_instruction = (uintptr_t)current+5;
             uint32_t displacement = 
            (current[1] |
            (current[2] << 8) |
            (current[3] << 16) |
            (current[4] << 24));
            uint8_t *target_addr = next_instruction + displacement;
            // uintptr_t offset = (uintptr_t )base - (uintptr_t )target_addr;
            printf("    jmp    0x%x \n",target_addr);
            break;
        default:
            printf("    Address 0x%08lx: 0x%08lx - ", (uintptr_t)current, value);
            printf("    Unhandled opcode [0x%x]\n", opcode);
            break;
        }

        // print_register_state(&regs); // Print the updated register state
        current += instruction_length;
    }
    free(stack);
    return 0;
}

