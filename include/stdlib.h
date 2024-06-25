#ifndef __STDLIB_H
#define __STDLIB_H

#include <stddef.h> // for size_t

// Declaration for calloc function
void* calloc(size_t num_elements, size_t element_size);

// Declaration for malloc function
void* malloc_int(size_t size);
void *malloc_wrap(size_t size,char function[1000]);
#define malloc(size) malloc_wrap(size, __func__)
// Declaration for free function
void *free(void *ptr);

// Declaration for realloc function
void* realloc(void* ptr, size_t new_size);

long strtol(const char* str, char** endptr, int base);

double ldexp(double x, int exp);
#endif