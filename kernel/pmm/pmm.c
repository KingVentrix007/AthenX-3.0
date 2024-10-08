#include "vmm.h"
#include "stdint.h"
#include "types.h"
#include "layout.h"
#include "multiboot.h"
#include "io_ports.h"
#include "pmm.h"
uint8_t pmm_paging_active = 0;

static uint32_t pmm_stack_loc = PMM_STACK_START;
static uint32_t pmm_stack_max = PMM_STACK_START;
static uint32_t pmm_location = 0;
static uint32_t pmm_stack_end = PMM_STACK_END;
// NOTE: if you ever wanted to **** up something here...
// pmm_location if physical but identity mapped
void init_pmm_page(uint32_t pmm_start,size_t size) {
	// Ensure the initial page allocation location is page-aligned.
	dbgprintf("Initializing pageing with %u pages\n",size/PAGE_SIZE);
	pmm_location = (pmm_start + PAGE_SIZE) & PAGE_ADDR_MASK;
	// pmm_stack_end = pmm_location + size;
}

uint32_t pmm_alloc_page() {
	// sanity check
	if (!pmm_paging_active) {
		dbgprintf("pmm_location alloc: 0x%.8x\n", pmm_location);

		uint32_t addr = pmm_location;
		pmm_location += PAGE_SIZE;
		return addr;
	}
	// sanity check
	if (PMM_STACK_START >= pmm_stack_loc)
		write_to_com1_string("PMM Stack: no free pages.");
	// pop
	pmm_stack_loc -= sizeof(uint32_t);
	uint32_t* stack = (uint32_t*) pmm_stack_loc;
	// NOTE: pmm_stack_loc is a pointer to a pointer TO the free page,
	// that is also the top of the stack, so we return VALUE
	// at the stack's top, not pointer to it.
	return *stack;
}

void pmm_free_page(uint32_t p) {
	p &= PAGE_ADDR_MASK;
	// ignore any page under "location", it contains some important crap
	// and is identity mapped (as all first 4 MB)
	if (p < pmm_location)
		return;
	// If we've run out of space on the stack...
	if (pmm_stack_max <= pmm_stack_loc) {
		// sanity check
		if (pmm_stack_end <= pmm_stack_max)
			printf("PMM Stack: out of mem for free pages.");
			// for(;;);
		// Map the page we're currently freeing at the top of the free page stack.
		map(pmm_stack_max, p, PAGE_PRESENT | PAGE_WRITE);
		// Increase the free page stack's size by one page.
		pmm_stack_max += PAGE_SIZE;
	} else {
		// Else we have space on the stack, so push.
		uint32_t* stack = (uint32_t*) pmm_stack_loc;
		*stack = p;
		pmm_stack_loc += sizeof(uint32_t);
	}
}

void pmm_collect_pages(MULTIBOOT_INFO* mboot_ptr) {
	uint32_t i = mboot_ptr->mmap_addr;
	// debug
	// printf("ignore pages before: 0x%.8x\nusable ram:\n", pmm_location);
	while (i < mboot_ptr->mmap_addr + mboot_ptr->mmap_length) {
		MULTIBOOT_MEMORY_MAP *me = (MULTIBOOT_MEMORY_MAP*) i;
		// usable ram?
		if (me->type == 1) {
			// debug
			// printf("0x%.8x\t0x%.8x\n", me->addr_low,
			// 		me->addr_low + me->len_low);
			uint32_t j;
			// For every page in this entry, add to the free page stack.
			for (j = me->addr_low; j < me->addr_low + me->len_low;
					j += 0x1000) {
				pmm_free_page(j);
			}
		}
		// The multiboot specification is strange in this respect:
		// the size member does not include "size" itself in its calculations,
		// so we must add sizeof (uint32_t).
		i += me->size + sizeof(uint32_t);
	}
}
/**
 * Function Name: pmm_alloc_pages
 * Description: Allocates a specific number of consecutive pages.
 *
 * Parameters:
 *   num_pages (uint32_t) - Number of consecutive pages to allocate.
 *
 * Return:
 *   uint32_t - Pointer to the start of the allocated memory block.
 */
uint32_t pmm_alloc_pages(uint32_t num_pages) {
    // Sanity check
    uint32_t start = pmm_alloc_page();
	uint32_t va_start = start;
	map(va_start,start,PAGE_PRESENT | PAGE_WRITE);
	uint32_t va_adder = va_start;
	for (size_t i = 0; i < num_pages-PAGE_SIZE; i++)
	{
		uint32_t adder = pmm_alloc_page();
		va_adder += PAGE_SIZE;
		map(va_adder,adder,PAGE_PRESENT | PAGE_WRITE);
	}
	return va_start;
	
}