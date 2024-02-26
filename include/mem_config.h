/**
 * @file mem_config.h
 * @brief Configuration settings for memory management.
 */

#ifndef MEM_CONFIG_H
#define MEM_CONFIG_H

/**
 * @brief Block size for memory management.
 */
#define BLOCK_SIZE 0x1000

/**
 * @brief Maximum number of cached allocations.
 */
#define MAX_CACHED_ALLOCATIONS 100

/**
 * @brief Alignment of teh memory region in bytes
 * 
 */

#define ALIGNMENT 16
/**
 * @brief Default log level for memory management.
 */
#define DEFAULT_LOG_LEVEL 2

/**
 * @brief Use verbose output for memory management.
 *
 * If USE_VERBOSE_OUTPUT is set to 1, the MEMORY_ALLOCATION_VERBOSE_OUTPUT macro is also defined.
 */
#define USE_VERBOSE_OUTPUT 0

#if USE_VERBOSE_OUTPUT == 1
/**
 * @brief Macro for verbose output in memory management.
 */
#define MEMORY_ALLOCATION_VERBOSE_OUTPUT
#endif
/**
 * @brief The Size of a block in the physical memory manager
 * 
 */


/**
 * Header File: my_header_file.h
 * Description: This header file defines PMM_BLOCK_SIZE_BYTES based on the architecture.
 */

// Check if ARCHTURE is defined
#define ARCH_X86
  // Check for specific architectures and set PMM_BLOCK_SIZE_BYTES accordingly
  #if defined(ARCH_X86)
    #define PMM_BLOCK_SIZE_BYTES 4096 // Set to the correct value for x86 architecture
  #elif defined(ARCH_ARM)
    #define PMM_BLOCK_SIZE_BYTES 8192 // Set to the correct value for ARM architecture
  #elif defined(ARCH_MIPS)
    #define PMM_BLOCK_SIZE_BYTES 8192 // Set to the correct value for MIPS architecture
  #else
    #error "Unsupported architecture" // Display an error for unsupported architectures
  #endif





#endif /* MEM_CONFIG_H */
