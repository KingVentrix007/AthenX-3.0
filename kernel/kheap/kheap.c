#include <kheap.h>
#include <pmm.h>
#include <vmm.h>
#include "io_ports.h"
#include "vesa.h"
/**
 * @brief Pointer to the start of the memory region.
 */
void* memory_region;

/**
 * @brief Pointer to the end of the node region.
 */
void* end_of_node_region;

/**
 * @brief Pointer to the end of the memory region.
 */
void* memory_region_end;

/**
 * @brief Error code for memory allocation.
 */
int memory_allocation_error_code;

/**
 * @brief Array of free memory zones.
 */
FreeZone free_zones[MAX_CACHED_ALLOCATIONS] = {};

/**
 * @brief Number of free memory zones.
 */
int free_zone_count = 0;

bool heap_active = false;

void init_kheap(uint32_t size)
{
    int64_t num_total_pages = size / PAGE_SIZE; // Total number of pages in the memory region
    uint64_t num_pages_40_percent = (num_total_pages * 0.4); // 40% of the total pages
    printf_com("Using %d pages for kheap\n", num_pages_40_percent);
	void *zone = pmm_alloc_pages(num_pages_40_percent);
    printf_com("Initializing memory region 0x%08X\n",zone);
	init_memory_allocation(zone,num_pages_40_percent*PAGE_SIZE);
    heap_active = true;
	
}

/**
 * @brief Prints information about a given node.
 *
 * This function prints various information about a given node, such as its address,
 * size, allocated status, first block status, and the number of blocks used.
 *
 * @param node A pointer to the node for which information needs to be printed.
 */
void print_node_info(const Node *node)
{
    MEM_ALLOC_LOG(2, "  Address: %p\n", node->addr);
    MEM_ALLOC_LOG(2, "  Size: %zu\n", node->size);
    MEM_ALLOC_LOG(2, "  Allocated: %s\n", node->allocated ? "true" : "false");
    MEM_ALLOC_LOG(2, "  First Block: %s\n", node->first_block ? "true" : "false");
    MEM_ALLOC_LOG(2, "  Num Blocks Used: %zu\n", node->num_block_used);
    MEM_ALLOC_LOG(2, "\n");
}

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
void init_memory_allocation(void *start_addr, size_t size)
{
    printf_com("Setting up region of size %u\n", size);
    init_memory_region(start_addr, size);
    // memory_allocations = (MemoryAllocationInfo *)sys_allocate_memory(sizeof(MemoryAllocationInfo) * 10);
    // Now nodes are initialized, and the available memory is divided accordingly
}
/**
 * @brief Allocates a consecutive bunch of memory blocks and updates the linked list.
 * @param size The size of memory to allocate in bytes.
 * @return The starting address of the allocated memory, or NULL if the allocation fails.
 */
void *sys_allocate_memory(int size)
{
    
    #if RUN_CHECKS_ON_ALLOCATE == 1
    run_checks();

    #endif
    
    // Calculate the number of blocks needed
    if (size > (memory_region_end - memory_region))
    {
        MEM_ALLOC_LOG(0, "Size is too large");
        return NULL;
    }
    int num_blocks_needed = (size + (BLOCK_SIZE-1)) / BLOCK_SIZE; // Round up to the nearest whole block

    // MEM_ALLOC_LOG(2, "Allocating %d blocks of memory\n", num_blocks_needed);

    // Find a consecutive bunch of free nodes
    Node *current_node = (Node *)memory_region;
    current_node = current_node->next;
    return find_free_zone(current_node,size,num_blocks_needed);
    
    MEM_ALLOC_LOG(0, "Failed to allocate %d bytes of memory\n", size);
    return NULL;
}
/**
 * @brief Frees memory blocks starting from the specified address and updates the linked list.
 *        Also removes the corresponding block from the memory_allocations list and sorts it.
 * 
 * This function is responsible for freeing memory blocks starting from the specified address and updating the linked list accordingly.
 * It takes the address of the memory to free as a parameter.
 * The function searches for the node corresponding to the given address in the linked list.
 * If the node is found and the memory is allocated, the function marks the memory blocks as free in the linked list.
 * It also updates the necessary fields in the nodes.
 * Additionally, the function creates a FreeZone structure to store information about the freed memory block,
 * and adds it to the free_zones array.
 * If the address is not found or the memory is not allocated, a log message is printed.
 * The function returns NULL after freeing the memory blocks.
 *
 * @param addr The address of the memory to free.
 * @return NULL after freeing the memory blocks, or the original address if it is not found or not allocated.
 */
void *sys_free_memory(const void *addr)
{
     #ifdef STANDALONE_MEMORY_ALLOCATION
    clock_t start_time = clock();
    #endif
    #if RUN_CHECKS_ON_FREE == 1
    run_checks();

    #endif
    Node *current_node = (Node *)memory_region;

    // Find the node corresponding to the given address
    while (current_node != NULL && current_node->addr != addr)
    {
        current_node = current_node->next;
    }

    FreeZone freezone;
    if (current_node != NULL && current_node->addr != NULL)
    {
        freezone.start_ptr = current_node->addr;
    }
    // freezone.size = get_memory_size(addr);

    // If the node is found, free the memory blocks
    if (current_node != NULL && current_node->allocated)
    {
        size_t num_blocks_to_free = current_node->num_block_used;

        // Mark the nodes as free
        for (int i = 0; i < num_blocks_to_free; ++i)
        {
            void *tmp_adder = current_node->addr;
            // unmap(current_node->addr);
            current_node->addr = tmp_adder;
            current_node->allocated = false;
            current_node->first_block = false;
            current_node->num_block_used = 0;
            current_node->total_requested_memory = 0;
            // memset(current_node->addr,0,BLOCK_SIZE);
            current_node = current_node->next;
        }

        // Return NULL after freeing
        // memset(addr, 0, get_memory_size(addr));
        #ifdef STANDALONE_MEMORY_ALLOCATION
        clock_t end_time = clock();
        long double elapsed_time = ((long double)(end_time - start_time)) / CLOCKS_PER_SEC;
        // printf("Elapsed Time: %Lf seconds to free\n", elapsed_time);
        elapsed_times_free[num_samples_free] = elapsed_time;
        num_samples_free++;
        #endif
        return NULL;
    }
    else
    {
        MEM_ALLOC_LOG(0, "Invalid address or memory is not allocated 0x%p\n", addr);

        // Return the original address if not found or not allocated
        return addr;
    }
}


/**
 * @brief Reallocates memory for a previously allocated block and updates the linked list.
 * @warning This function deallocates the addr value.
 *
 * This function is responsible for reallocating memory for a previously allocated block and updating the linked list accordingly.
 * It takes the address of the memory block to reallocate, the current size of the memory block, and the new size to which the memory block should be reallocated.
 * The function first checks for invalid parameters, such as when the old size exceeds the new size or when the address is NULL or out of range.
 * If the parameters are valid, the function allocates new memory using the `sys_allocate_memory` function.
 * It then checks if the allocation was successful, and if so, it copies the data from the old memory to the new memory using `memcpy`.
 * Finally, the function frees the old memory using the `sys_free_memory` function and returns the new address of the reallocated memory block.
 * If the reallocation fails, a log message is printed, and the function returns the original address.
 *
 * @param addr The address of the memory block to reallocate.
 * @param old_size The current size of the memory block in bytes.
 * @param new_size The new size to which the memory block should be reallocated.
 * @return The new address of the reallocated memory block, or the original address if the reallocation fails.
 */
void *sys_reallocate_memory(void *addr, int old_size, int new_size)
{
    // Check for invalid parameters
    if (old_size >= new_size)
    {
        MEM_ALLOC_LOG(1, "Reallocation of memory unnecessary as the old size exceeds the new size\n");
        return addr;
    }
    else if (addr == NULL || (addr < memory_region || addr > memory_region_end))
    {
        MEM_ALLOC_LOG(0, "Address is NULL\n");
        return NULL;
    }

    // Allocate new memory
    void *new_addr = sys_allocate_memory(new_size);

    // Check if the allocation was successful
    if (new_addr != NULL)
    {
        // Copy data from the old memory to the new memory
        memcpy(new_addr, addr, old_size);

        // Free the old memory
        sys_free_memory(addr);
        return new_addr;
    }
    else
    {
        MEM_ALLOC_LOG(0, "Failed to reallocate memory. Returning original address.\n");
        return addr;
    }
}
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
void print_memory_info()
{
    Node *current_node = (Node *)memory_region;
    MEM_ALLOC_LOG(2, "Memory Information:\n");

    size_t free_space = 0;

    while (current_node != NULL)
    {
        MEM_ALLOC_LOG(2, "  Address: %p\n", current_node->addr);
        MEM_ALLOC_LOG(2, "  Size: %zu\n", current_node->size);
        MEM_ALLOC_LOG(2, "  Allocated: %s\n", current_node->allocated ? "true" : "false");
        MEM_ALLOC_LOG(2, "  First Block: %s\n", current_node->first_block ? "true" : "false");
        MEM_ALLOC_LOG(2, "  Num Blocks Used: %zu\n", current_node->num_block_used);
        MEM_ALLOC_LOG(3, "\n");

        if (current_node->allocated == true)
        {
            MEM_ALLOC_LOG(3, "\n----------------------------------------------------------------\n");
            MEM_ALLOC_LOG(2, "Free space: %d\n", free_space);
            free_space = 0;
        }
        else
        {
            free_space = free_space + current_node->size;
        }

        current_node = current_node->next;
    }
}

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
void *init_memory_region(void *start_addr, size_t size) {
    // Calculate the number of nodes based on the size of each nodememory_region
    memory_region = start_addr;
    size_t num_nodes = size / BLOCK_SIZE;
    memory_region_end = memory_region + size;

    // Calculate the size needed for the nodes
    size_t bytes_used_by_nodes = num_nodes * sizeof(Node);

    // Ensure proper alignment for the start of the allocation region
    size_t alignment_padding = (ALIGNMENT - ((size_t)start_addr % ALIGNMENT)) % ALIGNMENT;
    void *aligned_start_addr = (char *)start_addr + alignment_padding;

    // Calculate the size available for actual allocation
    size_t bytes_available_for_allocation = size - bytes_used_by_nodes - alignment_padding;

    MEM_ALLOC_LOG(2, "%d nodes \n", num_nodes);
    MEM_ALLOC_LOG(2, "%d bytes are taken up by the nodes in memory\n", bytes_used_by_nodes);
    MEM_ALLOC_LOG(2, "%d bytes are available for actual allocation\n", bytes_available_for_allocation);

    // Cast the start address to Node pointer
    Node *node1 = (Node *)memory_region;

    // Calculate the end of the node region
    end_of_node_region = (void *)((char *)aligned_start_addr + bytes_used_by_nodes);

    // Calculate the start of the allocation region
    void *start_of_allocation_region = end_of_node_region;

    node1->num_block_used = node1->num_block_used + num_nodes;

    // Initialize the rest of the nodes
    Node *current_node = (Node *)aligned_start_addr;
    void *map_adder;
    for (int i = 1; i < num_nodes; ++i) {
        if (start_of_allocation_region + i * BLOCK_SIZE > aligned_start_addr + bytes_available_for_allocation) {
            node1->num_block_used = i;
            MEM_ALLOC_LOG(2, "Bytes available = %ld\n", node1->num_block_used * BLOCK_SIZE);
            break;
        }
        
        current_node->addr = start_of_allocation_region + i * BLOCK_SIZE;
        current_node->size = BLOCK_SIZE;
        current_node->next = (i < num_nodes - 1) ? (Node *)((char *)current_node + sizeof(Node)) : NULL;
        current_node->allocated = false;
        current_node->first_block = false;
        current_node->num_block_used = 0;
        if(i%4 == 0)
        {
            map_adder = current_node->addr;
            map(map_adder,map_adder,PAGE_WRITE|PAGE_PRESENT);
        }
        ++current_node;
    }

    return end_of_node_region;
}
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
void extend_allocation_space(void *extra_region, size_t size)
{
    //! CURRENTLY BROKEN
    Node *last_node = (Node *)memory_region;

    // Find the last node in the existing list
    while (last_node->next != NULL)
    {
        last_node = last_node->next;
    }

    // Initialize the new nodes in the extended allocation space
    void *end_of_extended_node_region = init_memory_region(extra_region, size);

    // Update the next pointer of the last node to point to the first new node
    last_node->next = (Node *)extra_region;

    // Update the end_of_node_region to the end of the extended node region
    end_of_node_region = end_of_extended_node_region;
}

/**
 * @brief Retrieves the size of the allocated memory block pointed to by ptr.
 *
 * This function is responsible for retrieving the size of the allocated memory block pointed to by ptr.
 * It takes a pointer to the memory block as a parameter.
 * The function iterates through the linked list of nodes, comparing the addresses,
 * and returns the size of the allocated memory block if found.
 * If the memory block is not found or not allocated, a log message is printed, and -1 is returned.
 *
 * @param ptr Pointer to the memory block.
 * @return The size of the allocated memory block, or -1 if it is not found or not allocated.
 */
size_t get_memory_size(const void *ptr)
{
    Node *current_node = (Node *)memory_region;
    size_t run_size = current_node->num_block_used;

    for (size_t i = 0; i < run_size; i++)
    {
        if (current_node->addr == ptr && current_node->allocated == true)
        {
            return current_node->total_requested_memory;
        }
        current_node = current_node->next;
    }

    MEM_ALLOC_LOG(0, "Failed to find Allocated memory region at %p", ptr);
    return -1;
}

/**
 * @brief Cleans up the memory by resetting the allocated nodes.
 *
 * This function is responsible for cleaning up the memory by resetting the allocated nodes.
 * It iterates through the linked list of nodes, sets the allocated flag to false,
 * clears the memory of the node, sets the size and number of blocks used to 0,
 * and moves to the next node.
 *
 * @return None.
 */
void memcleanup()
{
    Node *current_node = (Node *)memory_region;

    while (current_node != NULL && current_node->next != NULL)
    {
        current_node->allocated = false;
        memset(current_node->addr, 0, current_node->size);
        current_node->size = 0;
        current_node->num_block_used = 0;
        current_node = current_node->next;
    }
}

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
int buffer_overflow_detector()
{
    Node *current_node = (Node *)memory_region;
    const void *ptr = NULL;

    while (current_node != NULL && current_node->next != NULL)
    {
        if (current_node->allocated == false)
        {
            char temp_buffer[1028];
            memset(temp_buffer, 0, sizeof(temp_buffer)); // Initialize temp_buffer to avoid undefined behavior
            memcpy(temp_buffer, current_node->addr, BLOCK_SIZE);

            // Check if the memory region contains non-zero bytes
            for (size_t i = 0; i < sizeof(temp_buffer); ++i)
            {
                if (temp_buffer[i] != 0)
                {
                    MEM_ALLOC_LOG(1, "Possible  buffer overflows at 0x%p. Possibly caused by allocation at 0x%p", current_node->addr, ptr);
                    return -1;
                }
            }
        }

        if (current_node->allocated == true)
        {
            ptr = current_node->addr;
        }

        current_node = current_node->next;
    }

    return 0; // No  buffer overflows detected
}
/** 
 * @brief Function to detect memory leaks. 
 * 
 * This function iterates through a linked list of memory regions and checks for memory leaks. 
 * It logs information about allocated memory regions and their sizes. 
 * 
 * @return void 
 */ 
void memory_leak_detector() {
    // Work in progress
    Node *current_node = (Node *)memory_region;

    while (current_node != NULL) {
        if (current_node->allocated == true) {
            MEM_ALLOC_LOG(2, "Memory region of size %lu is allocated", current_node->size);

            // Assuming `num_block_used` is the number of blocks in the region
            size_t num_blocks = current_node->num_block_used;

            for (size_t i = 0; i < num_blocks; i++) {
                current_node = current_node->next;
            }
        }

        // Move to the next node
        current_node = current_node->next;
    }
}
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
int find_dangling_pointer() {
    Node *current_node = (Node *)memory_region;
    

    while (current_node != NULL && current_node->next != NULL) {
        if (current_node->allocated == false) {
            // Check if the memory region contains non-zero bytes
           for (size_t i = 0; i < current_node->size; ++i) {
            if (*((char *)(current_node->addr) + i) != 0 && 
                (current_node->next == NULL || !current_node->next->allocated)) {
                // Check if the memory region borders an allocated region
                MEM_ALLOC_LOG(1, "Potential dangling pointer at 0x%p", current_node->addr);
                return -1;
            }
        }

        }

        

        // Move to the next node
        current_node = current_node->next;
    }

    return 0; // No potential dangling pointers detected
}


int* run_checks()
{
    static int rets[2];
    rets[0] = buffer_overflow_detector();
    rets[1] = find_dangling_pointer();
    return rets;
}

#ifdef STANDALONE_MEMORY_ALLOCATION


long double get_average_free_time()
{
    long double total_time = 0.0;

    for (size_t i = 0; i < num_samples_free; ++i)
    {
        total_time += elapsed_times_free[i];
    }

    return (num_samples_free > 0) ? total_time / num_samples_free : 0.0;
}
#endif

int is_heap_active()
{
    return heap_active;
}

void *kmalloc(size_t size)
{
    while(heap_active != true)
    {

    }
	return sys_allocate_memory(size);
}
void kfree(void *ptr)
{
	sys_free_memory(ptr);
}
/**
 * @brief Finds a free zone of memory to allocate.
 * @param current_node The current node to start searching from.
 * @param size The size of memory to allocate.
 * @return A pointer to the start of the allocated memory.
 */
void *find_free_zone(Node *current_node, size_t size,int num_blocks_needed)
{
    #ifdef STANDALONE_MEMORY_ALLOCATION
        clock_t start_time = clock();
    #endif
    while (current_node != NULL)
    {
        if (!current_node->allocated)
        {
            int consecutive_free_blocks = 0;
            Node *start_node = current_node;
            start_node->total_requested_memory = size;
            // Check for consecutive free blocks
            while (current_node != NULL && !current_node->allocated && consecutive_free_blocks < num_blocks_needed)
            {
                ++consecutive_free_blocks;
                current_node = current_node->next;
            }
            // If enough consecutive free blocks are found, allocate them
            if (consecutive_free_blocks >= num_blocks_needed)
            {
                // Mark the nodes as allocated
                current_node = start_node;
                // printf_com("Called %d times",num_blocks_needed);
                for (int i = 0; i < num_blocks_needed; ++i)
                {
                    void *tmp_adder = current_node->addr;
	// LOG_LOCATION;
                    
                    // map(tmp_adder,current_node->addr,PAGE_PRESENT|PAGE_WRITE);
	// LOG_LOCATION;
                    
                    current_node->allocated = true; 
                    current_node->first_block = (i == 0);
                    current_node->num_block_used = num_blocks_needed;
                    current_node = current_node->next;
                }
                // MEM_ALLOC_LOG(2, "Allocated %d bytes of memory at %p\n", size,start_node->addr);
                #ifdef STANDALONE_MEMORY_ALLOCATION
                    clock_t end_time = clock();
                    long double elapsed_time = ((long double)(end_time - start_time)) / CLOCKS_PER_SEC;
                    // printf("Elapsed Time: %Lf seconds to free\n", elapsed_time);
                    elapsed_times_allocate[num_samples_alloc] = elapsed_time;
                    num_samples_alloc++;
                #endif
                return start_node->addr;
            }
        }
        // Move to the next node
        if (current_node != NULL && current_node->next != NULL)
        {
            current_node = current_node->next;
        }
    }

    return NULL;
}

/**
 * Function Name: create_map
 * Description: Prints memory regions with their start and end addresses and allocation status.
 *
 * Parameters: None
 *
 * Return: None
 */
void create_map()
{
    // return;
    Node *current_node = (Node *)memory_region; // Starting node of the linked list
    void *start_address = NULL;
    void *end_address = NULL;
    bool allocated_status;
    bool changed_status;
    allocated_status = current_node->allocated;
    changed_status = current_node->allocated;
    start_address = current_node->addr;
    while (current_node)
    {
        printf_com("Block 0x%X is %s\n",current_node->addr,(current_node->allocated == true) ? "Allocated" : "Unallocated");
        if(allocated_status != changed_status)
        {
            printf("Alloacted %d | changed = %d\n",allocated_status,changed_status);
            changed_status = current_node->allocated;
            allocated_status = current_node->allocated;
            // current_node = current_node->next;
            end_address = current_node->addr;
            printf("Region of size %u: 0x%X to 0x%X is %s\n",end_address-start_address,start_address,end_address,(allocated_status == true) ? "Allocated" : "Unallocated");
            start_address = end_address;
            end_address = NULL;
            printf_com("\n--------------------------------\n");

        }
        else
        {
            allocated_status = current_node->allocated;
            current_node = current_node->next;
        }
        
    }
    
}

int print_allocated()
{
    Node* current_node = (Node*)memory_region;
    while (current_node != NULL)
    {
        if(current_node->allocated == true)
        {
            printf_com("Node at 0x%X allocated\n", current_node->addr);
        }
        current_node = current_node->next;
    }
    
}