
#include <stddef.h>
#include "kheap.h"
#include "stdlib.h"
#include "vmm.h"
#include "io_ports.h"
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
  
    return ptr;
}
void free(void *ptr)
{
    // __sync_lock_release();
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
long strtol(const char *nptr, int base) {
    long result = 0;
    int sign = 1;
    const char *ptr = nptr;

    // Handle optional sign
    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    // Handle base prefix
    if (base == 0 || base == 16) {
        if (*ptr == '0' && (*(ptr + 1) == 'x' || *(ptr + 1) == 'X')) {
            base = 16;
            ptr += 2;
        }
    }

    // Convert string to long integer
    if (base == 0) {
        base = (*ptr == '0' ? 8 : 10); // octal if starts with 0, decimal otherwise
    }

    while (*ptr != '\0') {
        int digit;
        if (*ptr >= '0' && *ptr <= '9') {
            digit = *ptr - '0';
        } else if (*ptr >= 'a' && *ptr <= 'z') {
            digit = *ptr - 'a' + 10;
        } else if (*ptr >= 'A' && *ptr <= 'Z') {
            digit = *ptr - 'A' + 10;
        } else {
            break; // Invalid character
        }

        if (digit >= base) {
            break; // Invalid digit for the base
        }

        result = result * base + digit;
        ptr++;
    }

    return result * sign;
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