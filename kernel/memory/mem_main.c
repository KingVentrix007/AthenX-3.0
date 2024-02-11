#include "pmm.h"
#include "mem.h"
#define ARCH_X86
#include "mem_config.h"
int mem_main(void *free_region_start_address,size_t size);
#ifdef STANDALONE_MEMORY_ALLOCATION

#ifndef AUTOMATED_TESTING_MEMORY_ALLOCATION

/**
 * @brief Main function for standalone memory allocation.
 *
 * This function allocates initial memory using malloc and initializes memory allocation.
 * It then runs the memory allocator test suite and frees the initial allocated memory.
 *
 * @return 0 if successful, 1 if there was a problem using malloc.
 */
#ifndef LIB_BUILD
int main()
{
    // Allocate initial memory
    void *region_to_use = malloc(BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE);
    ptr_is_null(region_to_use);
    mem_main(region_to_use, BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE);

    // Run the memory allocator test suite

    // Free the initial allocated memory
    free(region_to_use);
    return 0;
}
#endif
/**
 * @brief Main function for automated testing.
 *
 * This function is used for automated testing of memory allocation.
 * It allocates initial memory using malloc and initializes memory allocation.
 *
 * @return 0 if successful, 1 if there was a problem using malloc.
 */
int main_automated_testing()
{
    void *region_to_use = malloc(BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE);
    ptr_is_null(region_to_use);
    mem_main(region_to_use, BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE);
    return 0;
}

/**
 * @brief Function to clean up after automated testing.
 *
 * This function ensures that memory_region is initialized before freeing it.
 * It frees the allocated memory and sets memory_region to NULL to avoid a dangling pointer.
 *
 * @return 0 if successful.
 */
int main_automated_testing_end()
{
    // Ensure that memory_region is initialized before using it
    // if (memory_region != NULL)
    // {
    //     free(memory_region);
    //     memory_region = NULL; // Set to NULL after freeing to avoid dangling pointer
    // }
    return 0;
}

#else

/**
 * @brief Main function for automated testing.
 *
 * This function is used for automated testing of memory allocation.
 * It allocates initial memory using malloc and initializes memory allocation.
 *
 * @return 0 if successful, 1 if there was a problem using malloc.
 */
int main_automated_testing()
{
    void *region_to_use = malloc(BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE);
    ptr_is_null(region_to_use);
    mem_main(region_to_use, BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE);
    return 0;
}

/**
 * @brief Function to clean up after automated testing.
 *
 * This function ensures that memory_region is initialized before freeing it.
 * It frees the allocated memory and sets memory_region to NULL to avoid a dangling pointer.
 *
 * @return 0 if successful.
 */
int main_automated_testing_end()
{
    // Ensure that memory_region is initialized before using it
    // if (memory_region != NULL)
    // {
    //     free(memory_region);
    //     memory_region = NULL; // Set to NULL after freeing to avoid dangling pointer
    // }
    return 0;
}

#endif

#endif

int mem_main(void *free_region_start_address,size_t size)
{
    
    init_pmm(free_region_start_address,size);
    if(size <100*PMM_BLOCK_SIZE_BYTES)
    {
        const void *allocation_region = allocate_pmm_block(size/2);
        init_memory_allocation(allocation_region,size/2);
        return 0;
    }
    else
    {
         const void *allocation_region = allocate_pmm_block(50*PMM_BLOCK_SIZE_BYTES);
        init_memory_allocation(allocation_region,100*PMM_BLOCK_SIZE_BYTES);
        return 0;
    }
    
}

void *kmalloc(size_t size)
{
    return sys_allocate_memory(size);
}

void *kfree(const void *ptr)
{
    return sys_free_memory(ptr);
}

