#include <stdint.h>
#include "printf.h"
#define MAX_FORMATTED_SIZE 50
/**
 * @brief Converts a given size in bytes to the smallest possible human-readable unit.
 * 
 * @param bytes The size in bytes to be formatted (uint32_t).
 * 
 * @details This function converts the input size in bytes into a more readable format,
 * such as KB, MB, GB, etc. It iteratively divides the size by 1024 until the size is
 * less than 1024 or it runs out of predefined suffixes. The formatted size is then
 * printed with two decimal places and the appropriate unit suffix.
 */
char* formatBytes(uint32_t bytes) {
    // Array of suffixes for different units
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    
    // Initialize index for suffixes and size variable
    int suffixIndex = 0;
    double size = (double)bytes;  // Convert bytes to double for decimal places

    // Loop until size is less than 1024 or we run out of suffixes
    while (size >= 1024 && suffixIndex < 8) {
        suffixIndex++;  // Move to the next suffix (KB, MB, etc.)
        size /= 1024;   // Divide size by 1024 to convert to next unit
    }

    // Fixed-size array for the formatted string
    static char formattedSize[MAX_FORMATTED_SIZE];

    // Format the size with two decimal places and the appropriate suffix
    snprintf(formattedSize, MAX_FORMATTED_SIZE, "%.2f %s", size, suffixes[suffixIndex]);
    // printf("\nformattedSize %s\n",formattedSize);
    // Since snprintf guarantees null-termination and MAX_FORMATTED_SIZE is large enough,
    // no need to explicitly add '\0' at the end.

    // Return the formatted string
    return formattedSize;
}

char* formatBytes64(uint64_t bytes) {
    // Array of suffixes for different units
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    
    // Initialize index for suffixes and size variable
    int suffixIndex = 0;
    double size = (double)bytes;  // Convert bytes to double for decimal places

    // Loop until size is less than 1024 or we run out of suffixes
    while (size >= 1024 && suffixIndex < 8) {
        suffixIndex++;  // Move to the next suffix (KB, MB, etc.)
        size /= 1024;   // Divide size by 1024 to convert to next unit
    }

    // Fixed-size array for the formatted string
    static char formattedSize[MAX_FORMATTED_SIZE];

    // Format the size with two decimal places and the appropriate suffix
    snprintf(formattedSize, MAX_FORMATTED_SIZE, "%.2f %s", size, suffixes[suffixIndex]);
    // printf("\nformattedSize64 %s\n",formattedSize);

    // Return the formatted string
    return formattedSize;
}