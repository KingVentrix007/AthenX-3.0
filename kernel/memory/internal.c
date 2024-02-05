/** 
 * @file internal.c 
 * @brief Internal functions for memory allocation
 * @author Tristan Kuhn 
 * @copyright Tristan Kuhn
 * @date 2023-09-15 
 * @version 1.0
 */ 
 

#include "internal.h"
#include <stddef.h>
#include "mem_output.h"
/**
 * @brief Compares the first n bytes of memory areas s1 and s2.
 *
 * This function compares the content of two memory areas, s1 and s2, up to the specified number of bytes (n).
 * It uses a byte-by-byte comparison and returns the difference between the first differing byte.
 *
 * @param s1 (const void*) - Pointer to the first memory area.
 * @param s2 (const void*) - Pointer to the second memory area.
 * @param n  (size_t)      - Number of bytes to be compared.
 *
 * @return (int) - 0 if the memory areas are equal, < 0 if s1 is less than s2, > 0 if s1 is greater than s2.
 */
int internal_memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;
    while (n-- > 0) {
        if (*p1 != *p2) {
            return (*p1 - *p2);
        }
        p1++;
        p2++;
    }
    return 0; // Memory areas are equal
}

/**
 * @brief Copies n bytes from memory area src to memory area dest.
 *
 * This function copies the content of the source memory area (src) to the destination memory area (dest)
 * up to the specified number of bytes (n). It uses a byte-by-byte copy.
 *
 * @param dest (void*)        - Pointer to the destination memory area.
 * @param src  (const void*)  - Pointer to the source memory area.
 * @param n    (size_t)       - Number of bytes to be copied.
 *
 * @return (void*) - Pointer to the destination memory area (dest).
 */
void* internal_memcpy(const void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n-- > 0) {
        *d++ = *s++;
    }
    return dest;
}

/**
 * @brief Custom implementation of memset.
 *
 * This function sets the value of each byte in the specified memory area (s) to the given value (c)
 * up to the specified number of bytes (n). It uses a byte-by-byte assignment.
 *
 * @param s (void*)  - Pointer to the memory to set.
 * @param c (int)    - Value to set. Should be an unsigned char cast to int.
 * @param n (size_t) - Number of bytes to set.
 *
 * @return (void*) - Pointer to the memory area (s).
 */
void* internal_memset(const void* s, int c, size_t n) {
    unsigned char* p = (unsigned char *)s;
    while (n-- > 0) {
        *p++ = (unsigned char)c;
    }
    return s;
}

/** 
 * @brief Checks if a pointer is null. 
 * 
 * This function checks if the provided pointer (ptr) is null. 
 * If the pointer is null, it logs a message indicating that the pointer is null. 
 *
 * @note This function assumes the existence of a macro MEM_ALLOC_LOG for logging messages.
 *
 * @param ptr (void*) - Pointer to be checked. 
 * 
 * @return int 
 */ 
int ptr_is_null(const void* ptr) 
{ 
    if (ptr == NULL) 
    { 
        MEM_ALLOC_LOG(0, "pointer is null\n"); 
        return 1; 
    }
    return 0;
}