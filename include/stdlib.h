#ifndef __STDLIB_H
#define __STDLIB_H

#include <stddef.h> // for size_t

// Declaration for calloc function
void* calloc(size_t num_elements, size_t element_size);

// Declaration for malloc function
void* malloc(size_t size);

// Declaration for free function
void free(void* ptr);

// Declaration for realloc function
void* realloc(void* ptr, size_t new_size);

long strtol(const char *nptr, int base);

double ldexp(double x, int exp);
#endif