#ifndef ATHENX_STRING_H
#define ATHENX_STRING_H

#include <stddef.h>

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
size_t strlen(const char* str);
char* strchr(const char* str, int c);
char* strrchr(const char* str, int c);
char* strstr(const char* haystack, const char* needle);
char* strtok(char* str, const char* delim);
size_t strspn(const char* str, const char* accept);
size_t strcspn(const char* str, const char* reject);
void* memset(void* ptr, int value, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
int memcmp(const void* ptr1, const void* ptr2, size_t n);

#endif /* ATHENX_STRING_H */
