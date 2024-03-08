
#include <stddef.h>
#include "kheap.h"
#include "stdlib.h"
#include "vmm.h"
#include "io_ports.h"
uint32_t *allocation_ptrs;
uint32_t allocation_ptrs_count;
int init_allocation_system()
{
    allocation_ptrs = malloc(1024*2);
    memset(allocation_ptrs, 0, 1024*2);
    allocation_ptrs_count = 0;
    

}
void* calloc(size_t num_elements, size_t element_size) {
    size_t total_size = num_elements * element_size;
    void* ptr = malloc(total_size); // Allocate memory using malloc

    if (ptr != NULL) {
        // Initialize allocated memory to zero
        for (size_t i = 0; i < total_size; ++i) {
            *((char*)ptr + i) = 0; // Set each byte to zero
        }
    }

    return ptr;
}

void *malloc(size_t size) {
    // Calculate the number of pages needed
    // size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    // Allocate memory for the pages
    void *ptr = kmalloc(size);
    if(ptr <= 0x00000000)
    {
        return NULL;
    }
    // Map each page
    allocation_ptrs[allocation_ptrs_count] = ptr;
    allocation_ptrs_count++;
    return ptr;
}
void free(void *ptr)
{
    // __sync_lock_release();
    for (size_t i = 0; i < allocation_ptrs_count; i++)
    {
        if(ptr == allocation_ptrs[i])
        {
            allocation_ptrs[i] = NULL;

            for (size_t j = i; j < allocation_ptrs_count - 1; j++) {
                allocation_ptrs[j] = allocation_ptrs[j + 1];
            }
            allocation_ptrs[allocation_ptrs_count - 1] = NULL; // Set the last element to NULL
        }   allocation_ptrs_count--;
    }
    
     kfree(ptr);
    
}
void* realloc(void* ptr, size_t new_size) {
    // If ptr is NULL, equivalent to malloc(new_size)
    if (ptr == NULL) {
        LOG_LOCATION;
        void *ret_ptr = malloc(new_size);
        if(ret_ptr != NULL)
        {
            LOG_LOCATION;
            return ret_ptr;
        }
        else
        {
            LOG_LOCATION;
            return NULL;
        }
         
    }
    
    // If new_size is 0, equivalent to free(ptr) and return NULL
    if (new_size == 0) {
        LOG_LOCATION;
        free(ptr);
        return NULL;
    }
    
    // Allocate memory for the new size
    void* new_ptr = malloc(new_size);
    
    if (new_ptr != NULL) {
        LOG_LOCATION;
        // Copy the contents from the old memory block to the new memory block
        // The amount of data to be copied is the minimum of the old size and the new size
        // Use memcpy to perform the copy operation
        size_t old_size = sizeof(ptr);
        LOG_LOCATION;
        if (new_size < old_size) {
            LOG_LOCATION;
            old_size = new_size;
        }
        for (size_t i = 0; i < old_size; ++i) {
            LOG_LOCATION;
            *((char*)new_ptr + i) = *((char*)ptr + i);
        }
        
        // Free the old memory block
        LOG_LOCATION;
        free(ptr);
    }
    
    return new_ptr;
}

int kheap_shutdown()
{
    for (size_t i = 0; i < allocation_ptrs_count; i++)
    {
        kfree(allocation_ptrs[i]);
    }
    
    
}

/**
 * Function Name: strtol
 * Description: Converts a string to a long integer.
 *
 * Parameters:
 *   nptr (const char *) - Pointer to the string to be converted.
 *   base (int) - Base of the number system (e.g., 10 for decimal).
 *
 * Return:
 *   long - The converted long integer value.
 */
long strtol(const char* str, char** endptr, int base) {
    // Initialize variables to hold the result and sign
    long result = 0;
    bool negative = false;

    // Skip leading white spaces
    while (isspace(*str)) {
        str++;
    }

    // Check for a sign (+ or -)
    if (*str == '-') {
        negative = true;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Check for the base prefix (0x for hexadecimal)
    if (base == 0) {
        if (*str == '0') {
            str++;
            if (*str == 'x' || *str == 'X') {
                base = 16;
                str++;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    }

    // Convert the string to a long integer
    while (isalnum(*str)) {
        char c = *str;
        int digit;

        if (isdigit(c)) {
            digit = c - '0';
        } else if (isalpha(c)) {
            digit = tolower(c) - 'a' + 10;
        } else {
            break;  // Invalid character, stop parsing
        }

        if (digit >= base) {
            break;  // Invalid digit for the current base
        }

        result = result * base + digit;
        str++;
    }

    // Set endptr to the first character after the parsed number
    if (endptr != NULL) {
        *endptr = (char*)str;
    }

    // Apply the sign
    if (negative) {
        result = -result;
    }

    return result;
}
/**
 * Function Name: ldexp
 * Description: Computes x * (2^exp).
 *
 * Parameters:
 *   x (double) - The base value.
 *   exp (int) - The exponent value.
 *
 * Return:
 *   double - The result of x * (2^exp).
 */
double ldexp(double x, int exp) {
    return x * pow(2, exp);
}
