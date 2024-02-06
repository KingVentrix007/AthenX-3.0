// string_functions.h
#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H
#include <stddef.h>
/**
 * Function Name: _strlen
 * Description: Calculate the length of a string.
 *
 * Parameters:
 *   str (char*) - The input string.
 *
 * Return:
 *   (int) - Length of the string.
 */
int strlen(const char* str);

/**
 * Function Name: _strstr
 * Description: Find the first occurrence of a substring in a string.
 *
 * Parameters:
 *   haystack (char*) - The main string to search.
 *   needle (char*) - The substring to find.
 *
 * Return:
 *   (char*) - Pointer to the first occurrence of the substring, or NULL if not found.
 */
char* strstr(const char* haystack, const char* needle);

/**
 * Function Name: _strcpy
 * Description: Copy the contents of one string to another.
 *
 * Parameters:
 *   dest (char*) - The destination string.
 *   src (const char*) - The source string.
 *
 * Return:
 *   (char*) - Pointer to the destination string.
 */
char* strcpy(char* dest, const char* src);
/**
 * Function Name: memcpy
 * Description: Copy a block of memory from one location to another.
 *
 * Parameters:
 *   dest (void*) - Pointer to the destination memory.
 *   src (const void*) - Pointer to the source memory.
 *   size (size_t) - Number of bytes to copy.
 *
 * Return:
 *   (void*) - Pointer to the destination memory.
 */
void* memcpy(void* dest, const void* src, size_t size);

/**
 * Function Name: memcpy
 * Description: Copy a block of memory from one location to another.
 *
 * Parameters:
 *   dest (void*) - Pointer to the destination memory.
 *   src (const void*) - Pointer to the source memory.
 *   size (size_t) - Number of bytes to copy.
 *
 * Return:
 *   (void*) - Pointer to the destination memory.
 */
void* memcpy(void* dest, const void* src, size_t size);

/**
 * Function Name: memset
 * Description: Fill a block of memory with a particular value.
 *
 * Parameters:
 *   ptr (void*) - Pointer to the memory block.
 *   value (int) - Value to set (usually in unsigned char format).
 *   size (size_t) - Number of bytes to set.
 *
 * Return:
 *   (void*) - Pointer to the memory block.
 */
void* memset(void* ptr, int value, size_t size);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
char* strncpy(char* dest, const char* src, size_t n);
#endif // STRING_FUNCTIONS_H
