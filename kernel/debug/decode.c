#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Define the maximum length for function names
#define MAX_FUNCTION_NAME 64
#define MAX_FRAMES 128

// Structure to hold function information
typedef struct {
    uintptr_t func_address;
    char function_name[MAX_FUNCTION_NAME];
} FunctionInfo;

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
    printf("Register State:\n");
    printf("    EAX: 0x%08x\n", regs->eax);
    printf("    EBX: 0x%08x\n", regs->ebx);
    printf("    ECX: 0x%08x\n", regs->ecx);
    printf("    EDX: 0x%08x\n", regs->edx);
    printf("    ESI: 0x%08x\n", regs->esi);
    printf("    EDI: 0x%08x\n", regs->edi);
    printf("    ESP: 0x%08x\n", regs->esp);
    printf("    EBP: 0x%08x\n", regs->ebp);
}

// Function to print the stack frame
int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES], int num_found_functions, int error_code) {
    uint8_t *current = (uint8_t *)base;
    uint8_t *end = current + size;
    RegisterState regs = {0};

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
            return 0; // End of function
        case 0x89:
            if (current[1] == 0xe5) {
                instruction_length = 2;
                printf("    mov    %%esp, %%ebp\n");
                regs.ebp = regs.esp;
            } else if (current[1] == 0x45 && current[2] == 0xfc) {
                instruction_length = 3;
                printf("    mov    %%eax, -0x4(%%ebp)\n");
            } else if (current[1] == 0x45 && current[2] == 0xf4) {
                instruction_length = 3;
                printf("    mov    %%eax, -0xc(%%ebp)\n");
            } else if (current[1] == 0x15) {
                instruction_length = 6;
                uintptr_t addr = *(uintptr_t *)(current + 2);
                printf("    mov    %s, 0x%lx\n", "%%edx", addr);
            } else if (current[1] == 0xc2) {
                instruction_length = 2;
                printf("    mov    %s, %s\n", "%%eax", "%%edx");
                regs.eax = regs.edx;
            } else {
                printf("    Unhandled mov\n");
            }
            break;
        case 0x83:
            if (current[1] == 0xec) {
                instruction_length = 3;
                printf("    sub    $0x%x, %%esp\n", current[2]);
                regs.esp -= current[2];
            } else if (current[1] == 0xc4) {
                instruction_length = 3;
                printf("    add    $0x%x, %%esp\n", current[2]);
                regs.esp += current[2];
            } else if (current[1] == 0x7d && current[2] == 0xf4) {
                instruction_length = 4;
                printf("    cmpl   $0x%x, -0xc(%%ebp)\n", current[3]);
            } else {
                printf("    Unhandled 0x83 opcode\n");
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
                char func_name[MAX_FUNCTION_NAME] = "<unknown>";
                for (size_t i = 0; i < num_found_functions; i++) {
                    FunctionInfo *func = &functions[i];
                    if (func->func_address == target_address) {
                        memset(func_name, 0, MAX_FUNCTION_NAME);
                        strcpy(func_name, func->function_name);
                    }
                }
                
                printf("    call   0x%08lx <%s>\n", target_address, func_name);
            }
            break;
        case 0xc7:
            if (current[1] == 0x45 && current[2] == 0xf4) {
                instruction_length = 7;
                int32_t imm = *(int32_t *)(current + 3);
                printf("    movl   $0x%x, -0xc(%%ebp)\n", imm);
            } else if (current[1] == 0x05) {
                instruction_length = 10;
                uintptr_t addr = *(uintptr_t *)(current + 2);
                int32_t imm = *(int32_t *)(current + 6);
                printf("    movl   $0x%x, 0x%lx\n", imm, addr);
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
                printf("    Unhandled 0x85 opcode\n");
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
                printf("    Unhandled 0x0f opcode\n");
            }
            break;
        case 0x84:
            if (current[1] == 0xc0) {
                instruction_length = 2;
                printf("    test   %s, %s\n", "%%al", "%%al");
            } else {
                printf("    Unhandled 0x84 opcode\n");
            }
            break;
        default:
            printf("    Address 0x%08lx: 0x%08lx - ", (uintptr_t)current, value);
            printf("    Unhandled opcode [0x%x]\n", opcode);
            break;
        }

        // print_register_state(&regs); // Print the updated register state
        current += instruction_length;
    }

    return 0;
}
