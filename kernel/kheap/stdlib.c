
#include <stddef.h>
#include "kheap.h"
#include "stdlib.h"
#include "vmm.h"
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
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    // Allocate memory for the pages
    void *ptr = kmalloc(num_pages * PAGE_SIZE);

    // Map each page
    for (size_t i = 0; i < num_pages; ++i) {
        // Calculate the virtual address for this page
        void *va = (char *)ptr + (i * PAGE_SIZE);

        // Map the page
        map(va, ptr, PAGE_WRITE | PAGE_PRESENT);
    }

    return ptr;
}
void free(void *ptr)
{
     kfree(ptr);
     unmap(ptr);
}
void* realloc(void* ptr, size_t new_size) {
    // If ptr is NULL, equivalent to malloc(new_size)
    if (ptr == NULL) {
        return malloc(new_size);
    }
    
    // If new_size is 0, equivalent to free(ptr) and return NULL
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    
    // Allocate memory for the new size
    void* new_ptr = malloc(new_size);
    
    if (new_ptr != NULL) {
        // Copy the contents from the old memory block to the new memory block
        // The amount of data to be copied is the minimum of the old size and the new size
        // Use memcpy to perform the copy operation
        size_t old_size = sizeof(ptr);
        if (new_size < old_size) {
            old_size = new_size;
        }
        for (size_t i = 0; i < old_size; ++i) {
            *((char*)new_ptr + i) = *((char*)ptr + i);
        }
        
        // Free the old memory block
        free(ptr);
    }
    
    return new_ptr;
}