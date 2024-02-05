/** 
 * @file internal.h
 * @brief Internal functions for memory allocation definitions
 * @author Tristan Kuhn 
 * @copyright Tristan Kuhn
 * @date 2023-09-15 
 * @version 1.0
 */ 
#ifndef __INTERNAL__MEMORY_ALLOATION__H
#define __INTERNAL__MEMORY_ALLOATION__H
#include <stddef.h>
/**
 * Function Name: internal_memset
 * Description: Sets the first n bytes of the memory area pointed to by s to the specified value.
 *
 * Parameters:
 *   s (void*) - Pointer to the memory area.
 *   c (int)   - Value to be set.
 *   n (size_t) - Number of bytes to be set.
 *
 * Return:
 *   (void*) - Pointer to the memory area s.
 */
void* internal_memset(const void* s, int c, size_t n);
/**
 * Function Name: internal_memcpy
 * Description: Copies n bytes from memory area src to memory area dest.
 *
 * Parameters:
 *   dest (void*) - Pointer to the destination memory area.
 *   src  (const void*) - Pointer to the source memory area.
 *   n    (size_t) - Number of bytes to be copied.
 *
 * Return:
 *   (void*) - Pointer to the destination memory area dest.
 */
void* internal_memcpy(const void* dest, const void* src, size_t n);
/**
 * Function Name: internal_memcmp
 * Description: Compares the first n bytes of memory areas s1 and s2.
 *
 * Parameters:
 *   s1 (const void*) - Pointer to the first memory area.
 *   s2 (const void*) - Pointer to the second memory area.
 *   n  (size_t)      - Number of bytes to be compared.
 *
 * Return:
 *   (int) - 0 if the memory areas are equal, < 0 if s1 is less than s2, > 0 if s1 is greater than s2.
 */
int internal_memcmp(const void* s1, const void* s2, size_t n);

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
int ptr_is_null(const void* ptr);
#endif
