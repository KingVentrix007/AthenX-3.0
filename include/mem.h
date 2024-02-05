/** 
 * @file mem.h
 * @brief Memory Allocation Implementation Definitions 
 * @author Tristan Kuhn 
 * @copyright Tristan Kuhn
 * @date 2023-09-15 
 * @version 1.0  
 */ 
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include <stddef.h> // Include for size_t
#ifdef STANDALONE_MEMORY_ALLOCATION
#include "stdlib.h"
#include "time.h"
#include "stdio.h"
#endif
#include "string.h"
#include "stdbool.h"
#include "mem_config.h"
#include "mem_output.h"
// Defines


/**  
 * @brief Node structure for representing a memory region.  
 *  
 * This structure represents a memory region and contains the following fields:  
 * - addr: Starting address of the memory region.  
 * - size: Size of the memory region.  
 * - next: Pointer to the next node in the linked list.  
 * - allocated: Boolean flag indicating whether this region is currently allocated.  
 * - first_block: Boolean flag indicating if this is the first block in the memory region.  
 * - num_block_used: Number of blocks used (will be 0 if first_block is false).  
 * - total_requested_memory: Total requested memory for this block of list, not rounded to the nearest 1024.  
 */  
typedef struct Node {  
    void* addr;            /**< Starting address of the memory region. */ 
    size_t size;           /**< Size of the memory region. */ 
    struct Node* next;     /**< Pointer to the next node in the linked list. */ 
    bool allocated;        /**< Boolean flag indicating whether this region is currently allocated. */ 
    bool first_block;      /**< Boolean flag indicating if this is the first block in the memory region. */ 
    size_t num_block_used; /**< Number of blocks used (will be 0 if first_block is false). */ 
    size_t total_requested_memory; /**< Total requested memory for this block of list, not rounded to the nearest 1024. */ 
} Node;

/** 
 * @brief Structure for representing memory allocation information. 
 * 
 * This structure represents memory allocation information and contains the following fields: 
 * - size: Size of the allocation in bytes. This is the exact number of bytes requested. 
 * - allocated_size: Size of the allocated memory region in bytes. This is the size of the memory region rounded to the nearest 1024 (or the size specified by Node->size). 
 * - ptr: Pointer to the allocated memory region. 
 */ 
typedef struct { 
    int size;                 /**< Size of the allocation in bytes. */
    int allocated_size;       /**< Size of the allocated memory region in bytes. */
    void* ptr;                /**< Pointer to the allocated memory region. */
} MemoryAllocationInfo;
/** 
 * @brief Structure for representing a free memory zone. 
 * 
 * This structure represents a free memory zone and contains the following fields: 
 * - start_ptr: Pointer to the start of the free memory zone. 
 * - end_ptr: Pointer to the end of the free memory zone. 
 * - size: Size of the free memory zone in bytes. 
 */ 
typedef struct FreeZone { 
    void* start_ptr;   /**< Pointer to the start of the free memory zone. */
    void* end_ptr;     /**< Pointer to the end of the free memory zone. */
    size_t size;       /**< Size of the free memory zone in bytes. */
} FreeZone;
/**
 * @brief Initializes memory allocation.
 *
 * This function is responsible for initializing memory allocation.
 * It takes a starting address and size as parameters.
 * The function calls the `init_memory_region` function to initialize the memory region.
 * Additionally, there are commented lines of code that suggest allocating memory for `memory_allocations`,
 * but they are currently not being used.
 *
 * @param start_addr The starting address of the memory region.
 * @param size The size of the memory region.
 */
void init_memory_allocation(void * start_addr, size_t size);


/**
 * Function Name: sys_allocate_memory
 * Description: Allocates a consecutive bunch of memory blocks and updates the linked list.
 *
 * Parameters:
 *   size (int) - The size of memory to allocate in bytes.
 *
 * Return:
 *   void
 */
void *sys_allocate_memory(int size);

/**
 * Function Name: sys_free_memory
 * Description: Frees memory blocks starting from the specified address and updates the linked list.
 *
 * Parameters:
 *   addr (void*) - The address of the memory to free.
 *
 * Return:
 *   void
 */
void *sys_free_memory(const void *addr);
/**
 * Function Name: sys_reallocate_memory
 * Description: Reallocates memory for a previously allocated block and updates the linked list.
 *
 * Parameters:
 *   addr (void*) - The address of the memory block to reallocate.
 *   old_size (int) - The current size of the memory block in bytes.
 *   new_size (int) - The new size to which the memory block should be reallocated.
 *
 * Return:
 *   void* - The new address of the reallocated memory block.
 */
void *sys_reallocate_memory(void *addr, int old_size, int new_size);
/**
 * @brief Finds potential dangling pointers by checking for non-zero data
 *        in memory regions that do not border an allocated region.
 *
 * This function iterates through the linked list of nodes, checks if the memory
 * region is allocated or not, and if not, it checks for non-zero bytes in the memory
 * region. If non-zero bytes are found and the memory region does not border an allocated
 * region, it logs a message indicating a potential dangling pointer.
 *
 * @return 0 if no potential dangling pointers are found, -1 if potential issues are detected.
 */
int find_dangling_pointer();
/**
 * @brief Prints information about the memory.
 *
 * This function is responsible for printing information about the memory.
 * It takes a printing option as a parameter, which determines the level of detail in the output.
 * The function iterates through the linked list of nodes and prints various details about each node,
 * such as its address, size, allocated status, first block status, and the number of blocks used.
 * Additionally, the function calculates and prints the amount of free space between allocated blocks.
 * The level of detail in the output is determined by the printing option.
 *
 * @param print_option Determines the printing option.
 * @return None.
 */
void print_memory_info();
/**
 * @brief Extends the allocation space by initializing new nodes in the extra region.
 *
 * This function is responsible for extending the allocation space by initializing new nodes in the extra region.
 * It takes a pointer to the extra region and the size of the extra region as parameters.
 * The function currently has a warning indicating that it is broken and needs fixing.
 * The logic of the function involves finding the last node in the existing list,
 * initializing new nodes in the extended allocation space using the `init_memory_region` function,
 * updating the next pointer of the last node to point to the first new node,
 * and updating the end_of_node_region to the end of the extended node region.
 *
 * @param extra_region Pointer to the extra region for extending the allocation space.
 * @param size Size of the extra region.
 * @return None.
 */
void extend_allocation_space(void *extra_region,size_t size);
/**
 * @brief Initializes the memory region for allocation.
 *
 * This function is responsible for initializing the memory region for allocation.
 * It takes the starting address of the memory region and the size of the memory region as parameters.
 * The function calculates the number of nodes based on the size of each node and the total size of the memory region.
 * It then calculates the size needed for the nodes and the size available for actual allocation.
 * The function initializes the first node and sets the end of the node region and the start of the allocation region.
 * It then iterates through the rest of the nodes, setting their addresses, sizes, and other fields.
 * If the available space for allocation is exceeded, the function adjusts the number of blocks used and logs the available bytes.
 * Finally, the function returns the end address of the node region.
 *
 * @param start_addr The starting address of the memory region.
 * @param size The size of the memory region.
 * @return The end address of the node region.
 */
void *init_memory_region(void *start_addr,size_t size);
/**
 * @brief Prints information about a given node.
 *
 * This function prints various information about a given node, such as its address,
 * size, allocated status, first block status, and the number of blocks used.
 *
 * @param node A pointer to the node for which information needs to be printed.
 */
void print_node_info(const Node *node);

//Currently incomplete
int* run_checks();
/** 
 * Function Name: get_memory_size 
 * Description: Retrieves the size of the allocated memory block pointed to by ptr. 
 * 
 * !! WARNING !! 
 * None. 
 * 
 * Parameters: 
 *   ptr (void*) - Pointer to the memory block. 
 * 
 * Return: 
 *   size_t - The size of the allocated memory block. 
 */
size_t get_memory_size(const void *ptr);


int main_automated_testing();
int main_automated_testing_end();
/**
 * @brief Detects buffer overflows by checking for non-zero bytes in the allocated memory regions.
 *
 * This function is responsible for detecting  buffer overflowss by checking for non-zero bytes in the allocated memory regions.
 * It iterates through the linked list of nodes, checks if the memory region is allocated or not,
 * and if not, it copies the memory region to a temporary buffer and checks for non-zero bytes.
 * If non-zero bytes are found, a log message is printed indicating a possible  buffer overflows.
 * The function also keeps track of the last allocated memory block pointer for reference.
 * The logic involves iterating through the linked list, copying memory regions, and checking for non-zero bytes.
 *
 * @return 0 if no  buffer overflows is detected, -1 if a possible  buffer overflows is found.
 */
int buffer_overflow_detector();
#ifdef STANDALONE_MEMORY_ALLOCATION

long double get_average_free_time();
#endif
#endif // MEMORY_MANAGER_H