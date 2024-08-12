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
FunctionInfo found_functions[MAX_FRAMES];
int num_found_functions = 0;
const char *black_box_functions[] = {"_start"};
// const FunctionInfo* find_function(const char *buffer, size_t buffer_size, unsigned int address);

typedef struct {
    char type[MAX_LINE_LENGTH];
    char name[MAX_FUNCTION_NAME];
} ParameterInfo;
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
void removeTrailingWhitespace(char *str) {
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

// Function to remove leading whitespace from a string
void removeLeadingWhitespace(char *str) {
    char *src = str;
    char *dst = str;

    // Skip leading whitespace
    while (isspace((unsigned char)*src)) {
        src++;
    }

    // Copy non-whitespace characters
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0'; // Null-terminate the result
}
// Helper function to trim leading and trailing whitespace
void trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) { // All spaces?
        return;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    *(end + 1) = 0;
    
}
int index_of_char(const char *str, char c) {
    const char *pos = strchr(str, c);
    if (pos == NULL) {
        return -1; // Character not found
    }
    return pos - str; // Calculate index
}
// Function to parse parameter strings
int parse_parameters(const char *parms, ParameterInfo *params) {
    int count = 0;
    char buffer[MAX_LINE_LENGTH];
    strcpy(buffer, parms);
    if(strcmp(buffer,"void") == 0)
    {
        return 0;
    }
    char *token = strtok(buffer, ",");
    while (token != NULL && count < MAX_PARAMS) {
        removeLeadingWhitespace(token);
        removeTrailingWhitespace(token);
        int end_of_type_index = index_of_char(token,' ');
        while(token[end_of_type_index] == ' ' || token[end_of_type_index] == '*')
        {
            end_of_type_index++;
        }
        // printf("Token = [%s] [%d]\n",token,end_of_type_index);
        char type[MAX_LINE_LENGTH] = "";
        for (size_t i = 0; i < end_of_type_index; i++)
        {
            type[i] = token[i];
        }
        // printf("type = [%s]\n",type);
        // printf("type + 1 == [%s]\n",token + end_of_type_index);
        removeLeadingWhitespace(type);
        removeTrailingWhitespace(type);
        strcpy(params[count].type, type);
        
        strcpy(params[count].name, token + end_of_type_index);
        count++;
        token = strtok(NULL, ",");
    }
    return count;
}



// #define COOL_OUTPUT
const FunctionInfo* find_function(const char *buffer, size_t buffer_size, unsigned int address) {
    if(address > 0x01000000)
    {
        return NULL;
    }
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

#ifdef COOL_OUTPUT
    printf("Scanning functions...\n");
#endif

    while (line_ptr != NULL) {
        unsigned int func_address;
        char current_function_name[MAX_FUNCTION_NAME];
        char current_file_path[MAX_LINE_LENGTH];
        char current_params[1024];
        int line_num;
        // Parse the line to extract address, file path, function name, and parameters
        int ret = sscanf_(line_ptr, "%x:%[^:]:%[^:]:%[^:]:%d", &func_address, current_file_path, current_function_name, current_params,&line_num);
        if (ret == 5) {
#ifdef COOL_OUTPUT
            // Cool effect: Print and then remove the line
            printf("%s", line_ptr);
            for (size_t i = 0; i < strlen(line_ptr); ++i) {
                printf("\b");
            }
#endif
            // Check if the function address is less than or equal to the provided address
            if (func_address <= address && func_address > closest_address) {
                closest_address = func_address;
                // printf("Closest address == %x\n",closest_address);
                // printf("Closests address == %x\n",closest_address);
                // Update result if a closer match is found
                strcpy(result.function_name, current_function_name);
                strcpy(result.file_path, current_file_path);
                result.func_address = func_address;
                result.line_num = line_num;
                strcpy(result.parms, current_params);
            }
        }
        line_ptr = strtok(NULL, "\n");
    }

    free(buffer_copy);

    if (closest_address != 0) {
#ifdef COOL_OUTPUT
        printf("Function found: %s at address 0x%x in file %s with params (%s)\n", result.function_name, result.func_address, result.file_path, result.parms);
#endif
        return &result;
    } else {
#ifdef COOL_OUTPUT
        printf("No matching function found.\n");
#endif
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


void print_parameter_value(uintptr_t ebp, int param_offset, const char *type) {
    uintptr_t *param_ptr = (uintptr_t *)(ebp + param_offset);
    printf("\t");
    if (strcmp(type, "int") == 0) {
        int value = *(int *)param_ptr;
        printf("        Value: %d\n", value);
    } else if (strcmp(type, "char") == 0 || strcmp(type, "char*") == 0) {
        char value = *(char *)param_ptr;
        printf("        Value: %c\n", value);
    } else if (strcmp(type, "char *") == 0) {
        char *value = *(char **)param_ptr;
        printf("        Value: %s\n", value);
    }  else if (strstr(type, "char") && strchr(type, '*')) {
        // Handle strings and fixed-size arrays as strings
        char *value = (char *)param_ptr;
        printf("        Value: %s\n", value);
    }else if (strcmp(type, "uint8_t") == 0) {
        uint8_t value = *(uint8_t *)param_ptr;
        printf("        Value: %u\n", value);
    } else if (strcmp(type, "uint16_t") == 0) {
        uint16_t value = *(uint16_t *)param_ptr;
        printf("        Value: %u\n", value);
    } else if (strcmp(type, "uint32_t") == 0) {
        uint32_t value = *(uint32_t *)param_ptr;
        printf("        Value: %u\n", value);
    } else if (strcmp(type, "uint64_t") == 0) {
        uint64_t value = *(uint64_t *)param_ptr;
        printf("        Value: %llu\n", value);
    } else if (strcmp(type, "float") == 0) {
        float value = *(float *)param_ptr;
        printf("        Value: %f\n", value);
    } else if (strcmp(type, "double") == 0) {
        double value = *(double *)param_ptr;
        printf("        Value: %f\n", value);
    } else if (strstr(type, "struct") != NULL) {
        printf("        Value: Struct data type\n");
    } else {
        printf("        Value: Unknown type\n");
    }
}




size_t estimate_frame_size(uintptr_t current_ebp, uintptr_t previous_ebp) {
    if (current_ebp >= previous_ebp) {
        return 0; // Invalid frame size
    }
    return previous_ebp - current_ebp;
}
uint32_t *unwind_stack(REGISTERS *reg) {
    int MaxFrames = MAX_FRAMES;
    // printf("Eip %x\n",eip);
    uintptr_t eip = reg->eip;
    uintptr_t ebp = reg->ebp;
    uintptr_t prev_ebp = ebp;
    printf("Eip %x\n",eip);

    int esp_in = 0;
    int first_frame = 1; // Flag to mark the first frame

    printf("\nStack trace:\n");

    for (unsigned int frame = 0; frame < MaxFrames; ++frame) {
        FunctionInfo *info = find_function(debug_map, strlen(debug_map), eip);

        if (info) {
         

            if (first_frame) {
                printf("\033[1;31m => "); // Print in red color and mark with an arrow
                first_frame = 0; // Reset flag after marking the first frame
                info->eip = eip;
                info->is_error = true;
            } else {
                printf("\033[0m");
                printf("    ");
                info->eip = eip;
                info->is_error = false;
            }
            printf("Frame %d - Address 0x%08x corresponds to function(or variable): %s", frame, info->func_address, info->function_name);
            if(strcmp(info->function_name,"_start") == 0)
            {
                 printf(" <-- Function is a black_box function. Cannot be analyzed\n");
            
            }
            else
            {
                printf("\n");
            }
            printf("      Defined in file: %s", info->file_path);
            if(strcmp(info->function_name,"_start") != 0)
            {
                printf("\n");
                ParameterInfo params[MAX_PARAMS];
                int param_count = parse_parameters(info->parms, params);
                for (int i = 0; i < param_count; ++i) {
                    if (strcmp(params[i].type, "Unknown type") == 0) {
                        printf("        %s: Unknown type\n", params[i].name);
                    } else if (strcmp(params[i].type, "No type") == 0) {
                        // Handle No type case if needed
                    } else if (strstr(params[i].type, "struct") != NULL) {
                        printf("        %s: Struct data type\n", params[i].name);
                    } else {
                        printf("        %s: [%s]\n", params[i].name, params[i].type);
                        print_parameter_value(ebp, (i + 2) * sizeof(uintptr_t), params[i].type); // Parameters start at EBP + 8
                    }
                }
                found_functions[num_found_functions] = *info;
                num_found_functions++;
            }
           
            
            // print_stack_frame((uintptr_t*)info->func_address, sizeof(uintptr_t) * 16); // Example frame size, adjust as needed
        } else {
            printf("    Frame %d - No function found for address 0x%x\n", frame, eip);
        }
        dbgprintf("Completed analysis for frame %d\n",frame);
        if (!ebp || ebp <= eip) {
            break; // Stop if we encounter an invalid frame or reach the end
        }
        dbgprintf("Checked ebp for frame %d\n",frame);
        uintptr_t* stack_ptr = (uintptr_t*)ebp;
        uintptr_t old_eip = eip;
        eip = stack_ptr[1]; // Next EIP
        ebp = stack_ptr[0]; // Previous EBP
        dbgprintf("Set eip and ebp  for frame %d\n",frame);
        if (esp_in == 0) {
            esp_in = reg->esp;
        }
        dbgprintf("Checked and set esp_in for frame %d\n",frame);
        // Estimate and print the stack frame
        // size_t frame_size = estimate_frame_size(ebp, prev_ebp);
        // printf("Estimated frame size: %zu bytes\n", frame_size);

        // if(frame_size != 0)
        // {
        //     print_stack_frame((uintptr_t*)ebp, frame_size);
        // }
        printf("Frame %d\n",frame);
        prev_ebp = ebp; // Update previous EBP for the next iteration

    }
    // printf("End of unwind\n");
    printf("\n");
    return 0; // Placeholder return
}