#include "kernel.h"
#include "debug.h"
#include <stdarg.h>
#include "stdbool.h"
#include <stddef.h>
#include "stdlib.h"
#include "isr.h"
#define MAX_FUNCTION_NAME 128
#define MAX_LINE_LENGTH 256

typedef struct {
    char function_name[MAX_FUNCTION_NAME];
    char file_path[MAX_LINE_LENGTH];
    unsigned int func_address;
} FunctionInfo;
const FunctionInfo* find_function(const char *buffer, size_t buffer_size, unsigned int address);
bool verbose_output = true;
char *strndup(const char *str, size_t n);
void print_kernel_memory_map(KERNEL_MEMORY_MAP *memory_map) {
    printf("Kernel Memory Map:\n");
    printf("Kernel:\n");
    printf("  Kernel Start Address: 0x%x\n", memory_map->kernel.k_start_addr);
    printf("  Kernel End Address: 0x%x\n", memory_map->kernel.k_end_addr);
    printf("  Kernel Length: %u\n", memory_map->kernel.k_len);
    printf("  Text Start Address: 0x%x\n", memory_map->kernel.text_start_addr);
    printf("  Text End Address: 0x%x\n", memory_map->kernel.text_end_addr);
    printf("  Text Length: %u\n", memory_map->kernel.text_len);
    printf("  Data Start Address: 0x%x\n", memory_map->kernel.data_start_addr);
    printf("  Data End Address: 0x%x\n", memory_map->kernel.data_end_addr);
    printf("  Data Length: %u\n", memory_map->kernel.data_len);
    printf("  RoData Start Address: 0x%x\n", memory_map->kernel.rodata_start_addr);
    printf("  RoData End Address: 0x%x\n", memory_map->kernel.rodata_end_addr);
    printf("  RoData Length: %u\n", memory_map->kernel.rodata_len);
    printf("  BSS Start Address: 0x%x\n", memory_map->kernel.bss_start_addr);
    printf("  BSS End Address: 0x%x\n", memory_map->kernel.bss_end_addr);
    printf("  BSS Length: %u\n", memory_map->kernel.bss_len);
    printf("System:\n"); 
    printf("  Total Memory: %u bytes\n", memory_map->system.total_memory*1024);
    printf("Available:\n");
    printf("  Start Address: 0x%x\n", memory_map->available.start_addr);
    printf("  End Address: 0x%x\n", memory_map->available.end_addr);
    printf("  Size: %u bytes\n", memory_map->available.size);
}

int disable_verbose()
{
    verbose_output = false;
    return 0;
}

void printf_t(const char *format, ...) {

    if(verbose_output == true)
    {
        va_list args;
        va_start(args, format);

        // Add custom functionality here
        // For example, adding a prefix to each message
        // printf("[MyPrintf] ");

        // Call the original printf function with the provided arguments
        vprintf_(format, args);

        va_end(args);
    }
    

    
}

void walk_stack(struct stackframe *stk) {
    printf("Backtrace:\n");
    for (unsigned int frame = 0; stk && frame < 100; ++frame) {  // Limit to 100 frames to prevent infinite loops
        printf("  Return address: 0x%x\n", stk->eip);
        
        // Buffer and size of the debug map (assuming debug_map is globally defined or accessible)
        size_t buffer_size = strlen(debug_map);
        
        // Variables to store function name and file path
        char function_name[MAX_FUNCTION_NAME];
        char file_path[MAX_LINE_LENGTH];
        
        // Call find_function to get function name and file path
        const FunctionInfo *info = find_function(debug_map, buffer_size, stk->eip);
        
        if (info) {
            printf("  Address 0x%x corresponds to function: %s\n", stk->eip, info->function_name);
            printf("    Defined in file: %s\n", info->file_path);  // Print the file path
        } else {
            printf("  No function found for address 0x%x\n", stk->eip);
        }
        
        stk = stk->ebp;
    }
}

const FunctionInfo* find_function(const char *buffer, size_t buffer_size, unsigned int address) {
    static FunctionInfo result;
    memset(&result, 0, sizeof(FunctionInfo)); // Clear result structure

    // Allocate memory for buffer copy
    char *buffer_copy = malloc(buffer_size + 1);
    if (!buffer_copy) {
        return NULL;
    }
    memcpy(buffer_copy, buffer, buffer_size);
    buffer_copy[buffer_size] = '\0';

    char *line_ptr = strtok(buffer_copy, "\n");
    unsigned int closest_address = 0;
    while (line_ptr != NULL) {
        unsigned int func_address;
        char current_function_name[MAX_FUNCTION_NAME];
        char current_file_path[MAX_LINE_LENGTH];

        // Parse the line to extract address, function name, and path
        int ret = sscanf_(line_ptr, "%x:%[^:]:%s", &func_address, current_file_path, current_function_name);
        if ( ret== 3) {
            // printf("%s",line_ptr);
            // Check if the function address is less than or equal to the provided address
            if (func_address <= address && func_address > closest_address) {
                closest_address = func_address;
                // Update result if a closer match is found
                strcpy(result.function_name, current_function_name);
                strcpy(result.file_path, current_file_path);
                result.func_address = func_address;
            }
        }
        line_ptr = strtok(NULL, "\n");
    }

    free(buffer_copy);

    if (closest_address != 0) {
        return &result;
    } else {
        return NULL;
    }
}
size_t strnlen(const char *str, size_t max_len) {
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}
char *strndup(const char *str, size_t n) {
    size_t len = strnlen(str, n);
    char *dup = (char *)malloc(len + 1);
    if (dup) {
        memcpy(dup, str, len);
        dup[len] = '\0';
    }
    return dup;
}



uint32_t *unwind_stack(REGISTERS *reg) {
    int MaxFrames = 100;
    uintptr_t eip = reg->eip;
    uintptr_t ebp = reg->ebp;
    int esp_in = 0;
    int first_frame = 1; // Flag to mark the first frame

    printf("\nStack trace:\n");

    for (unsigned int frame = 0; frame < MaxFrames; ++frame) {
        const FunctionInfo *info = find_function(debug_map, strlen(debug_map), eip);

        if (info) {
            if (first_frame) {
                printf("\033[1;31m => "); // Print in red color and mark with an arrow
                first_frame = 0; // Reset flag after marking the first frame
            } else {
                printf("\033[0m");
                printf("    ");
            }
            printf("Frame %d - Address 0x%x corresponds to function: %s\n", frame, eip, info->function_name);
            printf("      Defined in file: %s\n", info->file_path);
        } else {
            printf("    Frame %d - No function found for address 0x%x\n", frame, eip);
        }

        if (!ebp || ebp <= eip) {
            break; // Stop if we encounter an invalid frame or reach the end
        }

        uintptr_t* stack_ptr = (uintptr_t*)ebp;
        uintptr_t old_eip = eip;
        eip = stack_ptr[1]; // Next EIP
        ebp = stack_ptr[0]; // Previous EBP

        if (esp_in == 0) {
            esp_in = reg->esp;
        }
    }

    return 0; // Placeholder return
}