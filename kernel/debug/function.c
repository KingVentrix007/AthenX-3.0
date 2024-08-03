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
#include <limits.h>
const FunctionInfo* find_function_with_biggest_address(const char *buffer, size_t buffer_size) {
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
    unsigned int biggest_address = 0;

#ifdef COOL_OUTPUT
    printf("Scanning functions...\n");
#endif

    while (line_ptr != NULL) {
        unsigned int func_address;
        char current_function_name[MAX_FUNCTION_NAME];
        char current_file_path[MAX_LINE_LENGTH];
        char current_params[1024];

        // Parse the line to extract address, file path, function name, and parameters
        int ret = sscanf_(line_ptr, "%x:%[^:]:%[^:]:%[^)]", &func_address, current_file_path, current_function_name, current_params);
        if (ret == 4) {
#ifdef COOL_OUTPUT
            // Cool effect: Print and then remove the line
            printf("%s", line_ptr);
            for (size_t i = 0; i < strlen(line_ptr); ++i) {
                printf("\b");
            }
#endif
            // Check if the function address is greater than the current biggest address
            if (func_address > biggest_address) {
                biggest_address = func_address;
                // Update result if a bigger address is found
                strcpy(result.function_name, current_function_name);
                strcpy(result.file_path, current_file_path);
                result.func_address = func_address;
                strcpy(result.parms, current_params);
            }
        }
        line_ptr = strtok(NULL, "\n");
    }

    free(buffer_copy);

    if (biggest_address != 0) {
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

const FunctionInfo* find_function_with_smallest_address(const char *buffer, size_t buffer_size) {
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
    unsigned int smallest_address = UINT_MAX;

#ifdef COOL_OUTPUT
    printf("Scanning functions...\n");
#endif

    while (line_ptr != NULL) {
        unsigned int func_address;
        char current_function_name[MAX_FUNCTION_NAME];
        char current_file_path[MAX_LINE_LENGTH];
        char current_params[1024];

        // Parse the line to extract address, file path, function name, and parameters
        int ret = sscanf_(line_ptr, "%x:%[^:]:%[^:]:%[^)]", &func_address, current_file_path, current_function_name, current_params);
        if (ret == 4) {
#ifdef COOL_OUTPUT
            // Cool effect: Print and then remove the line
            printf("%s", line_ptr);
            for (size_t i = 0; i < strlen(line_ptr); ++i) {
                printf("\b");
            }
#endif
            // Check if the function address is less than the current smallest address
            if (func_address < smallest_address) {
                smallest_address = func_address;
                // Update result if a smaller address is found
                strcpy(result.function_name, current_function_name);
                strcpy(result.file_path, current_file_path);
                result.func_address = func_address;
                strcpy(result.parms, current_params);
            }
        }
        line_ptr = strtok(NULL, "\n");
    }

    free(buffer_copy);

    if (smallest_address != UINT_MAX) {
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
