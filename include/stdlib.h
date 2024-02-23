#ifndef __STDLIB_H
#define __STDLIB_H

#include <stddef.h> // for size_t

// Declaration for my_calloc function
void* calloc(size_t num_elements, size_t element_size);

// Declaration for my_malloc function
void* malloc(size_t size);

// Declaration for my_free function
void free(void* ptr);

// Declaration for my_realloc function
void* realloc(void* ptr, size_t new_size);
#endif