#ifndef __PMM__H
#define __PMM__H

#include "stdint.h"
#include "stddef.h"
/**
 * Function Name: start
 * Description: Initializes the memory region with a specified size
 *
 * Parameters:
 *   None
 *
 * Return:
 *   int - Return status (0 for success)
 */
int start();

/**
 * Function Name: init_pmm
 * Description: Initializes the Physical Memory Manager (PMM)
 *
 * Parameters:
 *   start_adder (void*) - Start address of the memory region
 *   size (size_t) - Size of the memory region
 *
 * Return:
 *   None
 */
int init_pmm(void * start_adder, size_t size);

/**
 * Function Name: allocate_pmm_block
 * Description: Allocates a block of memory from the initialized PMM
 *
 * Parameters:
 *   size (int) - Size of the memory block to allocate
 *
 * Return:
 *   void* - Address of the allocated memory block
 */
void *allocate_pmm_block(int size);

/**
 * Function Name: pmm_free_block
 * Description: Frees a previously allocated memory block in the PMM
 *
 * Parameters:
 *   addr (const void*) - Address of the memory block to free
 *
 * Return:
 *   void* - Always returns NULL after freeing the memory block
 */
void *pmm_free_block(const void *addr);


#endif