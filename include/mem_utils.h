#ifndef __MEM_UTILS__H
#define __MEM_UTILS__H

/**
 * @brief Finds a free zone of memory to allocate.
 * @param current_node The current node to start searching from.
 * @param size The size of memory to allocate.
 * @return A pointer to the start of the allocated memory.
 */
void *find_free_zone(Node *current_node, size_t size,int num_blocks_needed);
#ifdef STANDALONE_MEMORY_ALLOCATION
long double get_average_allocation_time();
#endif
#endif