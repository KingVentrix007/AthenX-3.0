#include "pmm.h"
#include "mem.h"
#include "internal.h"
#include "mem_config.h"
#include "mem_utils.h"
void *pmm_node_region_end = NULL;
void *pmm_memory_region_end = NULL;
void *pmm_memory_region = NULL;





/**
 * @brief Initializes the Physical Memory Manager (PMM).
 *
 * This function initializes the PMM by setting up the memory region and nodes for allocation.
 * It calculates the number of nodes based on the size of each node and sets the start and end
 * addresses of the memory region. It also calculates the size needed for the nodes, ensures
 * proper alignment for the start of the allocation region, and calculates the size available
 * for actual allocation. The function then initializes the nodes by assigning addresses, sizes,
 * and other attributes. Finally, it logs the relevant information and returns the number of
 * blocks used for nodes.
 *
 * @param start_adder The starting address of the memory region.
 * @param size The size of the memory region.
 * @return Returns the number of blocks used for nodes.
 */
int init_pmm(void *start_adder, size_t size)
{
    // Calculate the number of nodes based on the size of each node
    pmm_memory_region = start_adder;
    size_t num_nodes = size / PMM_BLOCK_SIZE_BYTES;
    pmm_memory_region_end = pmm_memory_region + size;

    // Calculate the size needed for the nodes
    size_t bytes_used_by_nodes = num_nodes * sizeof(Node);

    // Ensure proper alignment for the start of the allocation region
    size_t alignment_padding = (ALIGNMENT - ((size_t)start_adder % ALIGNMENT)) % ALIGNMENT;
    void *aligned_start_addr = (char *)start_adder + alignment_padding;

    // Calculate the size available for actual allocation
    size_t bytes_available_for_allocation = size - bytes_used_by_nodes - alignment_padding;

    MEM_ALLOC_LOG(2, "%d nodes \n", num_nodes);
    MEM_ALLOC_LOG(2, "%d bytes are taken up by the nodes in memory\n", bytes_used_by_nodes);
    MEM_ALLOC_LOG(2, "%d bytes are available for actual allocation\n", bytes_available_for_allocation);

    // Cast the start address to Node pointer
    Node *node1 = (Node *)pmm_memory_region;

    // Calculate the end of the node region
    pmm_node_region_end = (void *)((char *)aligned_start_addr + bytes_used_by_nodes);

    // Calculate the start of the allocation region
    void *start_of_allocation_region = pmm_node_region_end;

    node1->num_block_used = node1->num_block_used + num_nodes;

    // Initialize the rest of the nodes
    Node *current_node = (Node *)aligned_start_addr;
    for (int i = 1; i < num_nodes; ++i) {
        if (start_of_allocation_region + i * PMM_BLOCK_SIZE_BYTES > aligned_start_addr + bytes_available_for_allocation) {
            node1->num_block_used = i;
            MEM_ALLOC_LOG(2, "Bytes available = %ld\n", node1->num_block_used * PMM_BLOCK_SIZE_BYTES);
            break;
        }
        current_node->addr = start_of_allocation_region + i * PMM_BLOCK_SIZE_BYTES;
        current_node->size = PMM_BLOCK_SIZE_BYTES;
        current_node->next = (i < num_nodes - 1) ? (Node *)((char *)current_node + sizeof(Node)) : NULL;
        current_node->allocated = false;
        current_node->first_block = false;
        current_node->num_block_used = 0;
        ++current_node;
    }
    return node1->num_block_used;
}

/**
 * @brief Allocates a memory block of the specified size.
 *
 * This function allocates a memory block of the given size. It checks if the size is valid and
 * within the available memory region. If the size is too small or too large, it logs an error and
 * returns NULL. Otherwise, it calculates the number of blocks needed to accommodate the size and
 * searches for a consecutive bunch of free nodes. If enough consecutive free blocks are found, it
 * marks them as allocated and returns the starting address of the allocated memory. If no
 * consecutive free blocks are found, it logs an error and returns NULL.
 *
 * @param size The size of the memory block to be allocated.
 * @return Returns the starting address of the allocated memory if successful, otherwise returns NULL.
 */
void *allocate_pmm_block(int size)
{
    if(size < PMM_BLOCK_SIZE_BYTES*2)
    {
         MEM_ALLOC_LOG(0, "Allocated Region Too Small");
                    return NULL;
    }
      if (size > (pmm_memory_region_end - pmm_memory_region))
    {
        MEM_ALLOC_LOG(0, "Size is too large");
        return NULL;
    }
    int num_blocks_needed = (size + 1023) / PMM_BLOCK_SIZE_BYTES; // Round up to the nearest whole block
    MEM_ALLOC_LOG(2, "Allocating %d blocks of memory\n", num_blocks_needed);
    // Find a consecutive bunch of free nodes
    Node *current_node = (Node *)pmm_memory_region;
    current_node = current_node->next;
    return find_free_zone(current_node,size,num_blocks_needed);
    MEM_ALLOC_LOG(0, "Failed to allocate %d bytes of memory\n", size);
    return NULL;
    }

/**
 * @brief Frees a memory block at the given address.
 *
 * This function frees the memory block at the specified address. It searches for the node
 * corresponding to the given address and marks the blocks as free. If the node is found and
 * the memory blocks are allocated, it frees them by setting the necessary flags and attributes
 * to their default values. It also clears the memory content by calling memset(). If the node
 * is not found or the memory is not allocated, it logs an error and returns the original address.
 *
 * @param addr The address of the memory block to be freed.
 * @return Returns NULL if the memory blocks are successfully freed, otherwise returns the original address.
 */
void *pmm_free_block(const void *addr)
{
       Node *current_node = (Node *)pmm_memory_region;
    // Find the node corresponding to the given address
    while (current_node != NULL && current_node->addr != addr)
    {
        current_node = current_node->next;
    }
    






    
    if (current_node != NULL && current_node->allocated)
    {
        size_t num_blocks_to_free = current_node->num_block_used;
        // Mark the nodes as free
        for (int i = 0; i < num_blocks_to_free; ++i)
        {
            current_node->allocated = false;
            current_node->first_block = false;
            current_node->num_block_used = 0;
            current_node->total_requested_memory = 0;
            current_node = current_node->next;
        }
        // Return NULL after freeing
        memset(addr, 0, get_memory_size(addr));
       



        return NULL;
    }
    else
    {
        MEM_ALLOC_LOG(0, "Invalid address or memory is not allocated 0x%p\n", addr);
        // Return the original address if not found or not allocated
        return addr;
    }
}