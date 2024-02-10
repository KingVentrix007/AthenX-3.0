#ifndef VMM_H
#define VMM_H
#include "stdint.h"
#include "layout.h"
#define PAGE_SIZE 0x1000

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

#define PAGE_ADDR_MASK 0xFFFFF000
#define PAGE_FLAGS_MASK 0xFFF

#define PAGE_ROUND_DOWN(x) (((uint32_t)(x)) & PAGE_ADDR_MASK)
#define PAGE_ROUND_UP(x) ((((uint32_t)(x))+PAGE_SIZE-1) & PAGE_ADDR_MASK)

#define PG_NUM_FROM_ADDR(x) ((uint32_t)(x)>>12)
#define PG_FIRST_DIR_FROM_ADDR(x) ((uint32_t)((x)>>22)<<10)
#define PGDIR_IDX_FROM_ADDR(x) ((uint32_t)(x)>>22)
#define PGTAB_IDX_FROM_ADDR(x) (((uint32_t)(x)>>12) & 0x3FF)
#define PG_OFFSET_FROM_ADDR(x) ((uint32_t)(x) & PAGE_FLAGS_MASK)

#define NOT_PAGE_ALIGNED(va) PG_OFFSET_FROM_ADDR(va)
#define NOT_DIR_ENTRY_ALIGNED(va) (PGTAB_IDX_FROM_ADDR(va))

struct page_directory_mount {
	uint32_t* directory;
	uint32_t* pages; // linear, page-aligned address space
};
typedef struct page_directory_mount page_directory_mount_t;

struct page_directory {
	uint32_t physical; // physical address of page tables (cr3)
	// stores mount point of page directory in this directory
	uint32_t* directory;
	uint32_t* pages;
};
typedef struct page_directory page_directory_t;

// Sets up the environment, page directories etc and enables paging.
void init_vmm();

// Changes address space.
void switch_page_directory(page_directory_t* pd);

// Maps the physical page "pa" into the virtual space at address "va", using 
// page protection flags "flags".
void map(uint32_t va, uint32_t pa, uint32_t flags);

// Removes one page of V->P mappings at virtual address "va".
void unmap(uint32_t va);

// Returns 1 if the given virtual address is mapped in the address space.
// If "*pa" is non-NULL, the physical address of the mapping is placed in *pa.
uint8_t get_mapping(uint32_t va, uint32_t* pa);

// finds free range
uint32_t find_free_range(uint32_t va_start, uint32_t va_end, uint32_t size,
		uint8_t size_aligned);

// guarantees that whole range from va_start to va_end excluding is mapped
uint32_t allocate_range(uint32_t va_start, uint32_t va_end, uint32_t flags);

// Copies page directory returning it's virtual address and placing physical
// addresses in proper variables.
page_directory_t* clone_directory(page_directory_t* src);

// Deletes page directory freeing occupied memory
void destroy_directory(page_directory_t* dir);

void diff_directories(page_directory_t* adir, page_directory_t* bdir);

#endif