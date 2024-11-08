#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "debug.h"
#include "printf.h"
#include "stdlib.h"
#include "kernel.h"
// Define the maximum length for function names
// #define MAX_FUNCTION_NAME 64
#define MAX_FRAMES 128
FunctionInfo *smallest_function;
FunctionInfo *biggest_function;
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
typedef struct {
    uint32_t address;
    uint32_t data;
} MemoryEntry;
#define MEMORY_SIZE 1024
MemoryEntry memory[MEMORY_SIZE];
void initialize_decoder_memory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i].address = 0xFFFFFFFF;  // Use an invalid address to signify empty entry
        memory[i].data = 0;
    }
}
void print_string_with_limit(const char *str, int limit) {
    printf("[");
    for (int j = 0; str[j] != '\0'; j++) {
        if (j >= limit) {  // Limit to the specified number of characters
            printf("...");
            break;
        }
        if (str[j] == '\n') {
            printf("\\n");
        } else if (str[j] == '\t') {
            printf("\\t");
        } else {
            printf("%c",str[j]);
        }
    }
    printf("]");
}
void write_memory(uint32_t address, uint32_t data) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].address == address || memory[i].address == 0xFFFFFFFF) {
            memory[i].address = address;
            memory[i].data = data;
            return;
        }
    }
    printf("Memory overflow error\n");
}
uint32_t read_memory(uint32_t address) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].address == address) {
            return memory[i].data;
        }
    }
    printf("Memory read error: address not found\n");
    return 0;
}

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
int is_known_memory_address(uintptr_t addr,KERNEL_MEMORY_MAP kernel) {
    // Check if address is within known memory regions.
    // Replace the following checks with actual ranges from your memory map.
    if ((addr >= kernel.kernel.k_start_addr && addr <= kernel.kernel.k_end_addr) ||
        (addr >= kernel.kernel.text_start_addr && addr <= kernel.kernel.text_end_addr) ||
        (addr >= kernel.kernel.data_start_addr && addr <= kernel.kernel.data_end_addr) ||
        (addr >= kernel.kernel.rodata_start_addr && addr <= kernel.kernel.rodata_end_addr) ||
        (addr >= kernel.kernel.bss_start_addr && addr <= kernel.kernel.bss_end_addr)) {
        return 1; // Address is within known memory range.
    }
    return 0; // Address is not in known memory range.
}
int is_movl_string(uintptr_t addr)
{
    char *str = (const char *)addr;
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
                if(*str != '\n')
                {
                    return -1;
                }
                
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
int is_movl_allocation(uintptr_t addr)
{
    for (size_t i = 0; i <allocation_ptrs_count; i++)
    {
        if(addr == allocation_ptrs[i])
        {
            return 1;
        }
        
    }
    return -1;
    
}
int get_movl_type(int32_t imm)
{
    uintptr_t addr = (uintptr_t)imm;
    if(addr == 0)
    {
        return -1;
    }
    else if (addr <= biggest_function->func_address && addr >= smallest_function->func_address)
    {
        return 2;
    }
    else if(is_movl_string(addr) == 1)
    {
        return 1;
    }
    else if (is_movl_allocation(addr) == 1)
    {
        return 3;
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
const char* get_register_name_v2(uint8_t reg, uint8_t operand_size) {
    switch (operand_size) {
        case 8:
            switch (reg) {
                case 0x00: return "al";
                case 0x01: return "cl";
                case 0x02: return "dl";
                case 0x03: return "bl";
                case 0x04: return "ah";
                case 0x05: return "ch";
                case 0x06: return "dh";
                case 0x07: return "bh";
                default: return "unknown";
            }
        case 16:
            switch (reg) {
                case 0x00: return "ax";
                case 0x01: return "cx";
                case 0x02: return "dx";
                case 0x03: return "bx";
                case 0x04: return "sp";
                case 0x05: return "bp";
                case 0x06: return "si";
                case 0x07: return "di";
                default: return "unknown";
            }
        case 32:
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
        default:
            return "unknown";
    }
}
// To simulate a stack and use registers for your disassembler, you can indeed allocate a region of memory to act as the stack and then set one of the registers (like esp or ebp) to point to this allocated region. Here's a step-by-step approach to achieve this:

// 1. Allocate Memory for the Stack
// Use malloc to allocate a region of memory to simulate your stack:

// c
// Copy code
#define STACK_SIZE 4096
int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES], int error_code,uint32_t eip) {
    printf("A value of 0x0(0) or 0x0 as a parameter typically means that it is using the value on the stack, something not simulated here\n");
    if(base == NULL)
    {
        printf("Error: Base address is NULL\n");
        return -1;
    } 
    int push_count = 0;
    
    biggest_function = find_function_with_biggest_address(debug_map, strlen(debug_map));
    smallest_function = find_function_with_smallest_address(debug_map, strlen(debug_map));
    void *stack = malloc(size+100);
     if (stack == NULL) {
        printf("Error: Failed to allocate memory for stack\n");
        return -1;
    }
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
    while (1) 
    {
        uintptr_t value = *(uintptr_t *)current;
        uint8_t opcode = current[0];
        size_t instruction_length = 1;
        if(current == eip)
        {
            printf("\033[1;31m => ");
            
            // printf("==> ");
        }
        else
        {
            printf("\033[0m");
        }
        printf("%x - 0x%x",(uintptr_t)current,opcode);
        switch (opcode) {
        case 0x55:
            instruction_length = 1;

            printf("    push   %%ebp\n");
            regs.esp -= 4;  // Adjust stack pointer
            *(uintptr_t *)regs.esp = regs.ebp;  // Save %ebp to stack
            push_count+=1;
            break;
        case 0x50:
            instruction_length = 1;
            regs.esp -= 4;  // Adjust stack pointer
            *(uintptr_t *)regs.esp = regs.eax;  // Save %eax to stack
            printf("    push   %%eax\n");
            push_count+=1;

            break;
        case 0xc3:
            instruction_length = 1;
            printf("    ret\n");
            return 0; // End of function
        case 0x89: {
                uint8_t mod = (current[1] >> 6) & 0x03;
                uint8_t reg = (current[1] >> 3) & 0x07;
                uint8_t rm = current[1] & 0x07;
                push_count = 0;
                switch (mod) {
                    case 0x00: // No displacement
                        if (rm == 0x05) { // Direct address
                            instruction_length = 6;
                            uintptr_t addr = *(uintptr_t *)(current + 2);
                            printf("    mov    %%eax, 0x%lx\n", addr);
                            push_count = 0;
                        } else {
                            
                            printf("    Unhandled 0x89 opcode, ModR/M [mod=0x%x, rm=0x%x]\n", current[1], current[2]);
                            push_count = 0;

                        }
                        break;
                    case 0x01: // 8-bit displacement
                    case 0x02: // 32-bit displacement
                        instruction_length = 3 + (mod == 0x02 ? 4 : 1);
                        int32_t displacement = (mod == 0x01) ? (int8_t)current[2] : *(int32_t *)(current + 2);
                        if (rm == 0x05) {
                            printf("    mov    %%eax, %d(%%ebp)\n", displacement);
                         *(int32_t *)(regs.ebp + displacement) = regs.eax;
                            push_count = 0;

                        }
                        else if(current[1] == 0x50)
                        {
                            push_count = 0;
                            printf("    mov    %%edx, %d(%%eax)\n", displacement);
                        } 
                        else {
                            push_count = 0;
                            printf("    Unhandled 0x89 opcode with displacement, ModR/M [mod=0x%x, rm=0x%x]\n", current[1], current[2]);
                        }
                        instruction_length = 3 ;
                        break;
                    case 0x03: // Register mode
                        instruction_length = 2;
                        printf("    mov    %%%s, %%%s\n", get_register_name(reg),get_register_name(rm));
                        // mov    %esp, %ebp
                        //reg == esp
                        //rm = ebp
                        // printf("reg -- >0x%x\n",reg);
                        // printf("rm -- >0x%x\n",rm);

                        if(rm == 0x05 && reg == 0x04)
                        {
                            regs.ebp = regs.esp;
                        }
                        push_count = 0;

                        break;
                    default:
                        printf("    Unhandled 0x89 opcode, ModR/M [mod=0x%x, rm=0x%x]\n", current[1], current[2]);
                            push_count = 0;

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
                            push_count = 0;

                        } else {
                            printf("    Unhandled 0x8b opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                        }
                        break;
                    case 0x01:
                    case 0x02:
                        instruction_length = 2 + disp_size;
                        int32_t displacement = (mod == 0x01) ? (int8_t)current[2] : *(int32_t *)(current + 2);
                        printf("    mov     %d(%%ebp),%%%s\n", displacement,get_register_name(reg));
                        if(reg == 0x00)
                        {
                               regs.eax = *(int32_t *)(regs.ebp + displacement);
                            //   printf("Putting 0x%x in 0x%x\n", regs.eax, regs.ebp + displacement);
                        }
                            push_count = 0;
                        
                        break;
                    case 0x03:
                        instruction_length = 2;
                        printf("    mov    %%%s, %%%s\n", get_register_name(reg), get_register_name(rm));
                            push_count = 0;

                        break;
                    default:
                        printf("    Unhandled 0x8b opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                        break;
                }
                break;
            }

        case 0x83:
            // printf("Crash is here 0x%x\n",current[1] );
            push_count = 0;
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
                            push_count = 0;

                break;
        case 0xb9:
                instruction_length = 5;
                printf("    mov    $0x%x, %%ecx\n", *(int32_t *)(current + 1));
                regs.ecx = *(int32_t *)(current + 1);
                            push_count = 0;

                break;

        case 0x99:
            instruction_length = 1;
            printf("    cltd\n");
            break;
        case 0xf7:
            if (current[1] == 0xf9) {
                instruction_length = 2;
                
                printf("    idiv   %%ecx ");
                printf("%d/%d",regs.eax,regs.ecx);
                
                printf("\n");
            }else if ((current[1] & 0xf8) == 0x78) {
        // Handle cases like f7 7d f8 for idivl -0x8(%ebp)
        if ((current[1] & 0xc0) == 0x40) { // ModR/M byte with 8-bit displacement
            int8_t displacement = (int8_t)current[2];
            int32_t divisor  = *(int32_t *)((regs.ebp + displacement));
            int64_t  dividend = 0;//((int64_t)regs.edx << 32) | (uint32_t)regs.eax;
            // printf(" ((int64_t)regs.edx << 32) == [0x%x]\n", ((int64_t)regs.edx << 32));
            // printf(" (uint32_t)regs.eax == [0x%x]\n", (uint32_t)regs.eax);
            
            if(regs.edx == 0)
            {
                dividend = (int64_t)regs.eax;
            }
            else
            {
                dividend = ((int64_t)regs.edx << 32) | (uint32_t)regs.eax;
            }
            // printf("%d/",dividend);
            // printf("%d",divisor);
            printf("    idivl  %d(%%ebp) ", displacement);
            printf("%d/",dividend);
            printf("%d",divisor);
            // printf("    Dividend: %d\n", dividend);
            // printf("    Divisor: %d\n", divisor);
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
                // push_count = 0;
                if (push_count > 0) {
    printf("      \t\tStack contents at call. Num parms %d:\n",push_count);
    for (int i = 0; i < push_count; i++) {
        uintptr_t value = *(uintptr_t *)(regs.esp + (i * 4));
        int data_type = get_movl_type(value);

        printf("          \t\t- Parm %d --> 0x%lx(",i+1, value);
        
        if (data_type == 1) {
            char *str = (char *)value;
            print_string_with_limit(str,30);
            printf(")");
        } else if (data_type == 2) {
            FunctionInfo *func = find_function(debug_map, strlen(debug_map), value);
            if (func) {
                if (func->func_address == value) {
                    printf("[");
                    char *func_name = func->function_name;
                    for (int j = 0; func_name[j] != '\0'; j++) {
                        if (j >= 30) {  // Limit to 30 characters
                            printf("...");
                            break;
                        }
                        if (func_name[j] == '\n') {
                            printf("\\n");
                        } else if (func_name[j] == '\t') {
                            printf("\\t");
                        } else {
                            printf("%c",func_name[j]);
                        }
                    }
                    printf("]");
                }
                printf(")");
            }
        }
        else if (data_type == 3)
        {
            printf(">%p<)",value);
        }
        
        else
        {
            printf("%d)",value);
            if(value == 0)
            {
                printf(" <-- Not the actual value, exam source code");

            }
            // printf("Type cannot be decoded");
        }
        printf("\n");
       
    }
     push_count = 0;
}

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
                push_count = 0;

            } else {
                printf("    Unhandled 0xc7 opcode, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                            push_count = 0;

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
                        printf("    movl   $0x%x", imm);
                        print_string_with_limit(imm,30);
                        printf(", %d(%%ebp)\n",displacement);

                    }
                    else if (data_type == 2)
                    {
                        FunctionInfo *func = find_function(debug_map, strlen(debug_map),imm);
                        if(func)
                        {
                            if(func->func_address == imm)
                            {
                                printf("    movl   $0x%x(%s), %d(%%ebp)\n", imm,func->function_name, displacement);

                            }
                        }
                            // printf("    movl   $0x%x, %d(%%ebp)\n", imm, displacement);

                        

                    }
                    
                    else
                    {
                        printf("    movl   $0x%x(%d), %d(%%ebp)\n", imm,imm, displacement);
                    }
                    // regs.ebp = imm;
                    *(uintptr_t *)(regs.ebp + displacement) = imm;
                    // printf("%s",*(uintptr_t *)(regs.ebp + displacement) );
                    push_count = 0;
                    
                } else {
                    printf("    Unhandled 0xc7 opcode with displacement, ModR/M [mod=0x%x, rm=0x%x]\n", mod, rm);
                            push_count = 0;

                }
            }
            break;
        case 0x03: // Register mode
            printf("    movl   %%%s, %%%s\n", get_register_name(rm), get_register_name(reg));
                            push_count = 0;

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
                // printf("    push   -0xc(%%ebp)\n");
                int8_t local_displacement = (int8_t)current[2];  // 8-bit displacement
                uintptr_t local_address = regs.ebp + local_displacement;
                 printf("    push   -0x%x(%%ebp)\n", -local_displacement);
                if (local_address >= (uintptr_t)stack && local_address < (uintptr_t)stack + STACK_SIZE) {
                    uintptr_t local_value = *(uintptr_t *)local_address;
                   
                    // Ensure regs.esp is within the allocated stack range
                    if (regs.esp >= (uintptr_t)stack && regs.esp + 4 <= (uintptr_t)stack + STACK_SIZE) {
                        regs.esp -= 4;
                        *(uintptr_t *)regs.esp = local_value;
                        
                        push_count++;
                    } else {
                        printf("    Error: Stack pointer out of bounds\n");
                    }
                } else {
                    printf("    Error: Accessing invalid address 0x%lx\n", local_address);
                }
            } else {
                printf("    Unhandled 0xff opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0x53:
            instruction_length = 1;
            printf("    push   %%ebx\n");
            regs.esp -= 4;
            // printf("Stack 0x%x : push 0x%x\n",stack,*(uintptr_t *)regs.esp);
            *(uintptr_t *)regs.esp = regs.ebx;
            push_count++;
            break;
        case 0x5d:  // pop %ebp
            regs.ebp = *(uintptr_t *)regs.esp;  // Restore %ebp from stack
            regs.esp += 4;  // Adjust stack pointer
            printf("    pop    %%ebp\n");
            push_count--;
            break;
        case 0x68:
            {
                instruction_length = 5;
                regs.esp -= 4;
                *(uintptr_t *)regs.esp = *(int32_t *)(current + 1);  // Save immediate to stack
                int32_t imm = *(int32_t *)(current + 1);
                int data_type = get_movl_type(imm);
                if(data_type == 1)
                {
                printf("    push   $0x%x -->", imm,imm);
                print_string_with_limit(imm,30);
                printf("\n");

                }
                else if (data_type == 2)
                {
                    FunctionInfo *func = find_function(debug_map, strlen(debug_map),imm);
                    if(func)
                    {
                        if(func->func_address == imm)
                        {
                            printf("    push   $0x%x\n", imm,func->function_name);
                        }
                    }
                }
                
                else
                {
                    printf("    push   $0x%x(%d)\n", imm,imm);

                }
            push_count+=1;

            }
            break;
        case 0x6a:
            {
                instruction_length = 2;
                int8_t imm = *(int8_t *)(current + 1);
                int32_t imm32 = (int32_t)imm;
                regs.esp -= 4;
                push_count+=1;
                
        // Push the 32-bit immediate value onto the stack
                *(int32_t *)regs.esp = imm32;
                int data_type = get_movl_type(imm);
                if(data_type == 1)
                {
                // printf("    push   $0x%x(%s)\n", imm,imm);
                    printf("    push   $0x%x -->[%s]\n", imm,imm);


                }
                else if (data_type == 2)
                {
                    FunctionInfo *func = find_function(debug_map, strlen(debug_map),imm);
                    if(func)
                    {
                        if(func->func_address == imm)
                        {
                            printf("    push   $0x%x -->[%s]\n", imm,func->function_name);

                        }
                    }
                }
                
                else
                {
                    printf("    push   $0x%x(%d)\n", imm,imm);
                }
                
            }
            break;
        case 0xa3:
            {
                instruction_length = 5;

                uintptr_t addr = *(uintptr_t *)(current + 1);
                uint32_t address = *(uint32_t *)(current + 1);
                write_memory(address, regs.eax);
                printf("    mov    %%eax, 0x%lx\n", addr);
                push_count = 0;
            }
            break;
        case 0xa1:
            {
                instruction_length = 5;
                uintptr_t addr = *(uintptr_t *)(current + 1);
                printf("    mov    0x%lx, %%eax\n", addr);
                push_count = 0;
                regs.eax = read_memory(addr);
                push_count = 0;

            }
            break;
        case 0x85:
                push_count = 0;

            if (current[1] == 0xc0) {
                instruction_length = 2;
                printf("    test   %%eax, %%eax\n");
                
            } else {
                printf("    Unhandled 0x85 opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0x75:
            {
                push_count = 0;

                instruction_length = 2;
                int8_t offset = *(int8_t *)(current + 1);
                uintptr_t target_address = (uintptr_t)current + 2 + offset;
                printf("    jne    0x%lx\n", target_address);
            }
            break;
        case 0x74:
            {
                push_count = 0;

                instruction_length = 2;
                int8_t offset = *(int8_t *)(current + 1);
                uintptr_t target_address = (uintptr_t)current + 2 + offset;
                printf("    je     0x%lx\n", target_address);
            }
            break;
        case 0x0f:
                push_count = 0;

            if (current[1] == 0xb6 && current[2] == 0x00) {
                instruction_length = 3;
                printf("    movzbl (%s), %s\n", "%%eax", "%%eax");
            } else {
                printf("    Unhandled 0x0f opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0x84:
                push_count = 0;

            if (current[1] == 0xc0) {
                instruction_length = 2;
                printf("    test   %s, %s\n", "%%al", "%%al");
            } else {
                printf("    Unhandled 0x84 opcode, Second opcode [0x%x]\n",current[1]);
            }
            break;
        case 0xe9:
                push_count = 0;

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
        case 0xeb:
            instruction_length = 2;
            uint8_t *jmp_addr = current+instruction_length + current[1];
            printf("    jmp    0x%x \n",jmp_addr);
            push_count = 0;
            break;
        case 0x88: { // MOV r/m8, r8
    uint8_t modrm = current[1];
    uint8_t mod = (modrm >> 6) & 0x03;
    uint8_t reg = (modrm >> 3) & 0x07;
    uint8_t rm = modrm & 0x07;

    int32_t displacement = 0;
    size_t disp_size = 0;

    if (mod == 0x01) {
        disp_size = 1;
        displacement = (int8_t)current[2];
    } else if (mod == 0x02) {
        disp_size = 4;
        displacement = *(int32_t *)(current + 2);
    }

    instruction_length = 2 + disp_size;

    // Assuming `operand_size` is 8 for 8-bit operations
    const char *reg_name = get_register_name_v2(reg, 8);
    const char *rm_name = get_register_name_v2(rm, 8);

    if (mod == 0x00 && rm == 0x05) {
        // Special case for direct memory address (%eax)
        printf("    mov    %%%s, (%s)\n", reg_name, rm_name);

        // For demonstration, simulate memory update. Actual implementation might vary.
        // This example assumes `EAX` holds the memory address.
        if (reg == 0) {
            *(uint8_t *)(regs.eax) = regs.eax & 0xFF;
        } else if (reg == 1) {
            *(uint8_t *)(regs.eax) = regs.ecx & 0xFF;
        } else if (reg == 2) {
            *(uint8_t *)(regs.eax) = regs.edx & 0xFF;
        } else if (reg == 3) {
            *(uint8_t *)(regs.eax) = regs.ebx & 0xFF;
        } else if (reg == 4) {
            *(uint8_t *)(regs.eax) = regs.esp & 0xFF;
        } else if (reg == 5) {
            *(uint8_t *)(regs.eax) = regs.ebp & 0xFF;
        } else if (reg == 6) {
            *(uint8_t *)(regs.eax) = regs.esi & 0xFF;
        } else if (reg == 7) {
            *(uint8_t *)(regs.eax) = regs.edi & 0xFF;
        }
    } else if (mod == 0x01 || mod == 0x02) {
        // General case: move data to/from memory with displacement
        printf("    mov    %%%s, %d(%%%s)\n", reg_name, displacement, rm_name);

        // Simulate memory update based on `reg`
        if (rm == 0x05 && mod == 0x01) { // Example specific case
            if (reg == 0) {
                *(uint8_t *)(regs.ebp + displacement) = regs.eax & 0xFF;
            } else if (reg == 1) {
                *(uint8_t *)(regs.ebp + displacement) = regs.ecx & 0xFF;
            } else if (reg == 2) {
                *(uint8_t *)(regs.ebp + displacement) = regs.edx & 0xFF;
            } else if (reg == 3) {
                *(uint8_t *)(regs.ebp + displacement) = regs.ebx & 0xFF;
            } else if (reg == 4) {
                *(uint8_t *)(regs.ebp + displacement) = regs.esp & 0xFF;
            } else if (reg == 5) {
                *(uint8_t *)(regs.ebp + displacement) = regs.ebp & 0xFF;
            } else if (reg == 6) {
                *(uint8_t *)(regs.ebp + displacement) = regs.esi & 0xFF;
            } else if (reg == 7) {
                *(uint8_t *)(regs.ebp + displacement) = regs.edi & 0xFF;
            }
        }
    } else {
        printf("    Unhandled 0x88 opcode with ModR/M [mod=0x%x, reg=0x%x, rm=0x%x]\n", mod, reg, rm);
    }
    break;
}

        default:
            // printf("    Address 0x%08lx: 0x%08lx - ", (uintptr_t)current, value);
            printf("    Unhandled opcode [0x%x]\n", opcode);
            push_count = 0;
            break;
        }

        // print_register_state(&regs); // Print the updated register state
        if(current == eip)
        {
            printf("-----------------Code is not reached----------------\n");
            
            // printf("==> ");
        }
        current += instruction_length;
    }
    free(stack);
    printf("Function didn't reach past this point\n");
    return 0;
}

