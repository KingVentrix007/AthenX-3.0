#include <stddef.h>
#include "string.h"
/**
 * Copies the string pointed to by src, including the null terminator, 
 * to the buffer pointed to by dest.
 *
 * Parameters:
 *   dest (char*) - Pointer to the destination buffer where the string is to be copied.
 *   src (const char*) - Pointer to the source string to be copied.
 *
 * Return:
 *   (char*) - Pointer to the destination buffer (same as dest).
 */
char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while ((*dest++ = *src++) != '\0');
    return original_dest;
}

/**
 * Copies at most n characters from the string pointed to by src to the buffer 
 * pointed to by dest. If the length of src is less than n, the remainder of 
 * dest will be padded with null bytes.
 *
 * Parameters:
 *   dest (char*) - Pointer to the destination buffer where the string is to be copied.
 *   src (const char*) - Pointer to the source string to be copied.
 *   n (size_t) - Maximum number of characters to copy.
 *
 * Return:
 *   (char*) - Pointer to the destination buffer (same as dest).
 */
char* strncpy(char* dest, const char* src, size_t n) {
    char* original_dest = dest;
    while (n-- > 0 && (*dest++ = *src++) != '\0');
    while (n-- > 0) *dest++ = '\0'; // Padding with null bytes if necessary
    return original_dest;
}

/**
 * Concatenates the string pointed to by src to the end of the string 
 * pointed to by dest.
 *
 * Parameters:
 *   dest (char*) - Pointer to the destination string.
 *   src (const char*) - Pointer to the source string to be appended.
 *
 * Return:
 *   (char*) - Pointer to the destination string (same as dest).
 */
char* strcat(char* dest, const char* src) {
    char* original_dest = dest;
    while (*dest != '\0') dest++; // Move to the end of dest
    while ((*dest++ = *src++) != '\0'); // Copy src to the end of dest
    return original_dest;
}

/**
 * Concatenates at most n characters from the string pointed to by src to the end 
 * of the string pointed to by dest. A null terminator is always appended to dest.
 *
 * Parameters:
 *   dest (char*) - Pointer to the destination string.
 *   src (const char*) - Pointer to the source string to be appended.
 *   n (size_t) - Maximum number of characters to concatenate from src.
 *
 * Return:
 *   (char*) - Pointer to the destination string (same as dest).
 */
char* strncat(char* dest, const char* src, size_t n) {
    char* original_dest = dest;
    while (*dest != '\0') dest++; // Move to the end of dest
    while (n-- > 0 && (*dest++ = *src++) != '\0'); // Copy up to n characters from src to dest
    *dest = '\0'; // Ensure dest is null-terminated
    return original_dest;
}

/**
 * Compares two strings.
 *
 * Parameters:
 *   str1 (const char*) - Pointer to the first string.
 *   str2 (const char*) - Pointer to the second string.
 *
 * Return:
 *   (int) - Negative value if str1 < str2, 0 if str1 == str2, positive value if str1 > str2.
 */
int strcmp(const char* str1, const char* str2) {
    while (*str1 != '\0' && *str2 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    return (*str1 - *str2);
}

/**
 * Compares up to n characters of two strings.
 *
 * Parameters:
 *   str1 (const char*) - Pointer to the first string.
 *   str2 (const char*) - Pointer to the second string.
 *   n (size_t) - Maximum number of characters to compare.
 *
 * Return:
 *   (int) - Negative value if str1 < str2, 0 if str1 == str2, positive value if str1 > str2.
 */
int strncmp(const char* str1, const char* str2, size_t n) {
    while (n-- > 0 && *str1 != '\0' && *str2 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    if (n == 0) return 0; // Reached the maximum number of characters to compare
    return (*str1 - *str2);
}

/**
 * Returns the length of the string pointed to by str, 
 * excluding the null terminator.
 *
 * Parameters:
 *   str (const char*) - Pointer to the string.
 *
 * Return:
 *   (size_t) - Length of the string.
 */
size_t strlen(const char* str) {
    size_t length = 0;
    while (*str++ != '\0') length++;
    return length;
}

/**
 * Finds the first occurrence of the character c in the string pointed to by str.
 *
 * Parameters:
 *   str (const char*) - Pointer to the string.
 *   c (int) - Character to search for.
 *
 * Return:
 *   (char*) - Pointer to the first occurrence of c in str, or NULL if not found.
 */
char* strchr(const char* str, int c) {
    while (*str != '\0') {
        if (*str == (char)c) return (char*)str;
        str++;
    }
    return NULL;
}

/**
 * Finds the last occurrence of the character c in the string pointed to by str.
 *
 * Parameters:
 *   str (const char*) - Pointer to the string.
 *   c (int) - Character to search for.
 *
 * Return:
 *   (char*) - Pointer to the last occurrence of c in str, or NULL if not found.
 */
char* strrchr(const char* str, int c) {
    char* last_occurrence = NULL;
    while (*str != '\0') {
        if (*str == (char)c) last_occurrence = (char*)str;
        str++;
    }
    return last_occurrence;
}

/**
 * Finds the first occurrence of the substring needle in the string haystack.
 *
 * Parameters:
 *   haystack (const char*) - Pointer to the string to search in.
 *   needle (const char*) - Pointer to the substring to search for.
 *
 * Return:
 *   (char*) - Pointer to the first occurrence of needle in haystack, or NULL if not found.
 */
char* strstr(const char* haystack, const char* needle) {
    if (*needle == '\0') return (char*)haystack; // Empty needle matches at the beginning
    while (*haystack != '\0') {
        const char* h = haystack;
        const char* n = needle;
        while (*n != '\0' && *h == *n) {
            h++;
            n++;
        }
        if (*n == '\0') return (char*)haystack; // Found a match
        haystack++;
    }
    return NULL; // Not found
}

/**
 * Splits the string str into tokens, using the delimiter delim.
 * This function maintains internal state between calls.
 *
 * Parameters:
 *   str (char*) - Pointer to the string to split. It is modified during the call.
 *   delim (const char*) - Pointer to the null-terminated delimiter string.
 *
 * Return:
 *   (char*) - Pointer to the next token found, or NULL if no more tokens are found.
 */
char* strtok(char* str, const char* delim) {
    static char* next_token = NULL; // Internal state to maintain between calls
    if (str == NULL && next_token == NULL) return NULL; // No more tokens left
    if (str != NULL) next_token = str;
    // Skip leading delimiters
    next_token += strspn(next_token, delim);
    if (*next_token == '\0') return NULL; // No more tokens left
    // Find end of token
    char* token_start = next_token;
    next_token += strcspn(next_token, delim);
    if (*next_token != '\0') {
        *next_token = '\0';
        next_token++;
    }
    return token_start;
}

/**
 * Calculates the length of the initial segment of the string str 
 * consisting of only the characters found in the string accept.
 *
 * Parameters:
 *   str (const char*) - Pointer to the string.
 *   accept (const char*) - Pointer to the null-terminated string containing the characters to accept.
 *
 * Return:
 *   (size_t) - Length of the initial segment of str consisting of only the characters found in accept.
 */
size_t strspn(const char* str, const char* accept) {
    size_t length = 0;
    while (*str != '\0' && strchr(accept, *str) != NULL) {
        length++;
        str++;
    }
    return length;
}

/**
 * Calculates the length of the initial segment of the string str 
 * consisting of only the characters not found in the string reject.
 *
 * Parameters:
 *   str (const char*) - Pointer to the string.
 *   reject (const char*) - Pointer to the null-terminated string containing the characters to reject.
 *
 * Return:
 *   (size_t) - Length of the initial segment of str consisting of only the characters not found in reject.
 */
size_t strcspn(const char* str, const char* reject) {
    size_t length = 0;
    while (*str != '\0' && strchr(reject, *str) == NULL) {
        length++;
        str++;
    }
    return length;
}

/**
 * Sets the first n bytes of the block of memory pointed by ptr 
 * to the specified value (interpreted as an unsigned char).
 *
 * Parameters:
 *   ptr (void*) - Pointer to the block of memory to fill.
 *   value (int) - Value to be set. The value is passed as an int, 
 *                 but the function fills the block of memory using 
 *                 the unsigned char conversion of this value.
 *   n (size_t) - Number of bytes to fill.
 *
 * Return:
 *   (void*) - Pointer to the memory block ptr.
 */
void* memset(void* ptr, int value, size_t n) {
    unsigned char* p = (unsigned char*)ptr;
    while (n-- > 0) *p++ = (unsigned char)value;
    return ptr;
}

/**
 * Copies n bytes from memory area src to memory area dest. 
 * The memory areas must not overlap.
 *
 * Parameters:
 *   dest (void*) - Pointer to the destination memory area.
 *   src (const void*) - Pointer to the source memory area.
 *   n (size_t) - Number of bytes to copy.
 *
 * Return:
 *   (void*) - Pointer to the destination memory area (same as dest).
 */
void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n-- > 0) *d++ = *s++;
    return dest;
}

/**
 * Compares the first n bytes of the memory areas pointed by ptr1 and ptr2.
 *
 * Parameters:
 *   ptr1 (const void*) - Pointer to the first memory area.
 *   ptr2 (const void*) - Pointer to the second memory area.
 *   n (size_t) - Number of bytes to compare.
 *
 * Return:
 *   (int) - Negative value if ptr1 < ptr2, 0 if ptr1 == ptr2, positive value if ptr1 > ptr2.
 */
int memcmp(const void* ptr1, const void* ptr2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    while (n-- > 0) {
        if (*p1 != *p2) return (*p1 - *p2);
        p1++;
        p2++;
    }
    return 0;
}
