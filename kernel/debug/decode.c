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
const char *register_names[8] = {"%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi"};
// Function to print the ModR/M-based address and calculate instruction length
int decode_modrm_address(uint8_t mod, uint8_t rm, int8_t disp8, int32_t disp32) {
    int length = 2;  // Start with opcode + ModR/M byte

    if (mod == 0) {  // No displacement or special cases
        if (rm == 5) {
            printf("(0x%x)", disp32);  // Direct 32-bit address
            length += 4;  // 32-bit displacement
        } else {
            printf("(%s)", register_names[rm]);
        }
    } else if (mod == 1) {  // 8-bit displacement
        printf("%d(%s)", disp8, register_names[rm]);
        length += 1;  // 8-bit displacement
    } else if (mod == 2) {  // 32-bit displacement
        printf("%d(%s)", disp32, register_names[rm]);
        length += 4;  // 32-bit displacement
    } else if (mod == 3) {  // Register direct addressing
        printf("%s", register_names[rm]);
    }

    return length;
}

// Disassemble `mov` instruction with opcode 0x89 and return length
int disassemble_mov(uint8_t *current) {
    uint8_t opcode = current[0];
    
    if (opcode == 0x89) {
        uint8_t modrm = current[1];
        uint8_t mod = (modrm & 0xC0) >> 6;  // Top 2 bits
        uint8_t reg = (modrm & 0x38) >> 3;  // Middle 3 bits (source register)
        uint8_t rm = modrm & 0x07;          // Bottom 3 bits (destination register/memory mode)
        
        // Print source register based on `reg` field
        printf("mov    %s, ", register_names[reg]);
        
        int length = 0;  // Initial length for opcode + ModR/M byte
        
        // Check addressing mode and displacement
        if (mod == 0 && rm == 5) {
            // 32-bit displacement (direct address)
            int32_t disp32 = *((int32_t*)&current[2]);
            length += decode_modrm_address(mod, rm, 0, disp32);
        } else if (mod == 1) {
            // 8-bit displacement
            int8_t disp8 = (int8_t)current[2];
            length += decode_modrm_address(mod, rm, disp8, 0);
        } else if (mod == 2) {
            // 32-bit displacement
            int32_t disp32 = *((int32_t*)&current[2]);
            length += decode_modrm_address(mod, rm, 0, disp32);
        } else if (mod == 3) {
            // Register direct mode
            length += decode_modrm_address(mod, rm, 0, 0);
        }
        
        printf("\n");  // New line after the decoded instruction
        return length;
    }
    
    return 0;  // Return 0 if it's not a recognized instruction
}

// Disassemble ALU instructions with opcode 0x83 (like `add`, `sub`, etc.)
int disassemble_alu(uint8_t *current) {
    uint8_t opcode = current[0];
    
    if (opcode == 0x83) {
        uint8_t modrm = current[1];
        uint8_t mod = (modrm & 0xC0) >> 6;  // Top 2 bits for mod
        uint8_t reg = (modrm & 0x38) >> 3;  // Middle 3 bits for reg/opcode (operation)
        uint8_t rm = modrm & 0x07;          // Bottom 3 bits for register/memory mode
        
        int length = 2;  // Start with opcode + ModR/M byte
        
        // Determine operation based on `reg` field in ModR/M byte
        switch (reg) {
            case 0:  // `add`
                printf("add    ");
                break;
            case 5:  // `sub`
                printf("sub    ");
                break;
            case 7:  // `cmpl`
                printf("cmpl   ");
                break;
            default:
                printf("unsupported ALU operation (reg field: %d)\n", reg);
                return 0;
        }
        
        // Check for addressing mode in `mod`
        if (mod == 3) {  // Register direct mode (no memory addressing)
            // Print register name
            printf("$0x%x, %s", current[2], register_names[rm]);
            length += 1;  // 1-byte immediate value
        } else if (mod == 1) {  // Memory addressing with 8-bit displacement
            int8_t disp8 = (int8_t)current[2];  // 8-bit signed displacement
            printf("$0x%x, %d(%s)", current[3], disp8, register_names[rm]);
            length += 2;  // 1-byte displacement + 1-byte immediate
        } else {
            printf("unsupported addressing mode\n");
            return 0;
        }
        
        printf("\n");  // New line after the decoded instruction
        return length;
    }
    
    return 0;  // Return 0 if it's not a recognized instruction
}

int disassemble_jne(uint8_t *current, uint32_t address) {
    uint8_t opcode = current[0];
    
    if (opcode == 0x75) {  // `jne` opcode for a short jump
        int8_t offset = (int8_t)current[1];  // 8-bit signed offset
        uint32_t target_address = address + 2 + offset;  // Next instruction address + offset
        
        printf("jne     0x%x\n", target_address);
        
        return 2;  // `jne` with 8-bit offset is 2 bytes long
    }
    
    return 0;  // Not a recognized `jne` instruction
}

// Disassemble `push` instruction with opcode 0x68 and return length
// int disassemble_push(uint8_t *current) {
//     uint8_t opcode = current[0];
    
   
    
//     return 0;  // Return 0 if it's not a recognized `push` instruction
// }
int disassemble_call(uint8_t *current, uint32_t instruction_address,uint32_t *adder) {
    uint8_t opcode = current[0];
    
    if (opcode == 0xE8) {
        // Extract the 32-bit relative offset in little-endian format
        int32_t offset = current[1] | (current[2] << 8) | (current[3] << 16) | (current[4] << 24);
        
        // Calculate the target address by adding the offset to the address of the next instruction
        uint32_t target_address = instruction_address + 5 + offset;
        *adder = target_address;
        printf("call   0x%x", target_address);
        
        // Length of the instruction: 1 byte for opcode + 4 bytes for offset = 5 bytes
        return 5;
    }
    
    return 0;  // Return 0 if it's not a recognized `call` instruction
}
int disassemble_mov_imm(uint8_t *current) {
    uint8_t opcode = current[0];
    
    // Check if opcode is between 0xb8 and 0xbf
    if (opcode >= 0xb8 && opcode <= 0xbf) {
        uint8_t reg_code = opcode - 0xb8;  // Register code based on opcode
        uint32_t immediate = current[1] | (current[2] << 8) | (current[3] << 16) | (current[4] << 24);  // Little-endian
        
        // Print the decoded instruction
        printf("mov    $0x%x, %s\n", immediate, register_names[reg_code]);
        
        return 5;  // Length of the instruction (1 byte for opcode, 4 bytes for immediate)
    }
    
    return 0;  // Not a recognized `mov` immediate instruction
}
int disassemble_jmp(uint8_t *current, uint32_t instruction_address)
{
    uint8_t opcode = current[0];
    if(opcode == 0xeb)
    {
        int32_t offset = current[1];
        uint32_t target_address = instruction_address + 2 + offset;
        printf("jmp    0x%x\n",target_address);
        return 2;
    }
    return 0;
}
// Function to disassemble the movl instruction
int disassemble_movl(uint8_t *current) {
    uint8_t opcode = current[0];
    int length = 1; // Start with opcode length

    if (opcode == 0xC7) {
        uint8_t modrm = current[1];
        uint8_t mod = (modrm & 0xC0) >> 6; // Top 2 bits for addressing mode
        uint8_t reg = (modrm & 0x38) >> 3; // Middle 3 bits for operation (0x0 for movl)
        uint8_t rm = modrm & 0x07;          // Bottom 3 bits for register/memory mode
        length++; // Include ModR/M byte

        // Ensure it's a `movl` operation (reg should be 0)
        if (reg == 0) {
            printf("movl   ");

            // Decode based on mod value
            if (mod == 0b00) { // No displacement
                if (rm == 0b101) { // Special case: disp32
                    int32_t disp32 = *(int32_t*)&current[2]; // Displacement
                    printf("$0x%x, %d\n", *(uint32_t*)&current[6], disp32);
                    length += 4; // 4 bytes for disp32
                    length += 4; // 4 bytes for immediate value
                } else {
                    printf("$0x%x, %s\n", *(uint32_t*)&current[2], register_names[rm]);
                    length += 4; // 4 bytes for immediate value
                }
            } else if (mod == 0b01) { // 8-bit displacement
                int8_t disp8 = (int8_t)current[2];
                printf("$0x%x, %d(%s)\n", *(uint32_t*)&current[3], disp8, register_names[rm]);
                length += 1; // 1 byte for disp8
                length += 4; // 4 bytes for immediate value
            } else if (mod == 0b10) { // 32-bit displacement
                int32_t disp32 = *(int32_t*)&current[2];
                printf("$0x%x, %d(%s)\n", *(uint32_t*)&current[6], disp32, register_names[rm]);
                length += 4; // 4 bytes for disp32
                length += 4; // 4 bytes for immediate value
            } else if (mod == 0b11) { // Register direct mode
                printf("$0x%x, %s\n", *(uint32_t*)&current[2], register_names[rm]);
            }

            return length; // Return the total length of the instruction
        }
    } 
    return 0; // Return 0 if it's not a recognized `movl` instruction
}

int disassemble_push(uint8_t *current) {
    uint8_t opcode = current[0];
    int length = 1;  // Start with opcode length

    if (opcode == 0xFF) {
        uint8_t modrm = current[1];
        uint8_t mod = (modrm & 0xC0) >> 6; // Top 2 bits for addressing mode
        uint8_t reg = (modrm & 0x38) >> 3; // Middle 3 bits for operation (0x6 for push)
        uint8_t rm = modrm & 0x07;         // Bottom 3 bits for register/memory mode
        length++; // Include ModR/M byte

        // Ensure it's a `push` operation (`reg` should be 6)
        if (reg == 6) {
            printf("push   ");

            if (mod == 0) { // No displacement
                if (rm == 5) { // Special case: disp32
                    int32_t disp32 = *(int32_t*)&current[2];
                    printf("0x%x", disp32);
                    length += 4; // 4 bytes for disp32
                } else {
                    printf("%s", register_names[rm]);
                }
            } else if (mod == 1) { // 8-bit displacement
                int8_t disp8 = (int8_t)current[2];
                printf("%d(%s)", disp8, register_names[rm]);
                length += 1; // 1 byte for disp8
            } else if (mod == 2) { // 32-bit displacement
                int32_t disp32 = *(int32_t*)&current[2];
                printf("%d(%s)", disp32, register_names[rm]);
                length += 4; // 4 bytes for disp32
            } else if (mod == 3) { // Register direct mode
                printf("%s", register_names[rm]);
            }
            
            printf("\n");
            return length;
        }
    } else if (opcode >= 0x50 && opcode <= 0x57) { // push reg (single-byte opcode for registers)
        uint8_t reg = opcode - 0x50;
        printf("push   %s\n", register_names[reg]);
        return 1; // Single byte instruction
    }
    else  if (opcode == 0x68) {
        // `push` with a 32-bit immediate value
        uint32_t immediate = current[1] | (current[2] << 8) | (current[3] << 16) | (current[4] << 24);
        
        printf("push   $0x%x\n", immediate);
        
        // Length of the instruction: 1 byte for opcode + 4 bytes for immediate = 5 bytes
        return 5;
    }
    else if (opcode == 0x6A) {  // `push` with an immediate 8-bit value
        uint8_t immediate = current[1];
        printf("push   $0x%x\n", immediate);
        return 2;  // 1-byte opcode + 1-byte immediate
    }
    return 0; // Return 0 if it's not a recognized `push` instruction
}

#define STACK_SIZE 4096
int print_stack_frame(uintptr_t *base, size_t size, FunctionInfo functions[MAX_FRAMES], int error_code,uint32_t eip) 
{
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
        printf("  %x:	",current);
        if(opcode == 0xc3)
        {
            //Return
            printf("ret\n");
            break;
        }
        else if (opcode == 0x55)
        {
            printf("push   %%ebp\n");

        }
        else if (opcode == 0x89)
        {
            instruction_length = disassemble_mov(current);
        }
        else if(opcode == 0x83)
        {
            instruction_length = disassemble_alu(current);
        }
        else if (opcode == 0x68)
        {
            instruction_length = disassemble_push(current);
        }
        else if (opcode == 0x6a)
        {
           printf("push   $0x%x\n",current[1]);
           instruction_length = 2;

        }
        else if(opcode == 0xe8)
        {
            uint32_t adder;
            instruction_length = disassemble_call(current,current,&adder);
            FunctionInfo *function = find_function(debug_map, strlen(debug_map),adder);
            printf(" <%s>\n",function->function_name);
            // find_function(debug_map, strlen(debug_map), eip);
            
        }
        else if(opcode == 0x75)
        {
            instruction_length = disassemble_jne(current,current);
        }
        else if(opcode >= 0xb8 && opcode <= 0xbf)
        {
            instruction_length = disassemble_mov_imm(current);
        }
        else if (opcode == 0xeb)
        {
            instruction_length = disassemble_jmp(current,current);
        }
        else if(opcode == 0xFF || opcode >= 0x50 && opcode <= 0x57)
        {
            instruction_length = disassemble_push(current);
        }
        else if (opcode == 0xc7)
        {
            instruction_length = disassemble_movl(current);
        }
        
        else
        {
            printf("Unknown opcode %x\n",opcode);
        }
        current +=instruction_length;
        

    }

    free(stack);
    return 0;
}

