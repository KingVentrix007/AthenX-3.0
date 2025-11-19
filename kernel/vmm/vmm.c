#include "stdint.h"
#include "isr.h"
#include "vmm.h"
#include "pagepmm.h"
#include "io_ports.h"
#include "stdbool.h"
#include "kheap.h"
#include "stdio.h"
void pmm_free_page(uint32_t p);
uint32_t pmm_alloc_page();
extern uint8_t pmm_paging_active;

page_directory_t* current_directory;

// NOTE: page_directory_t is not just raw address just in case,
// further we can add list of mounted directories or something.

// note that without identity mapping this variable won't be
// valid anymore after enabling paging
page_directory_t kernel_directory;
// NOTE: kernel directory is mounted at the same place in every directory

void page_fault(REGISTERS *regs);
bool can_map = false;
void init_vmm() {
	// page faults
	// isr_register_interrupt_handler(14, page_fault);

	// NOTE: remember that every entry in directory and tables have flags,
	// to obtain addresses must be and'ed with PAGE_ADDR_MASK

	// NOTE: beware if you're using pmm_alloc_page_zero() or plain pmm_alloc_page()

	// prepare kernel page directory
	uint32_t* dir_ptr = (uint32_t*) pmm_alloc_page();
	if(dir_ptr == NULL)
	{
		printf("Could not allocate directory\n");
	}
	memset(dir_ptr, 0, PAGE_SIZE);
	kernel_directory.physical = (uint32_t) dir_ptr;

	// TODO: migrate with whole kernel to HIGH_MEM
	// (to set up in this mapping)

	// identity mapping for first 4 MB
	uint32_t* tab_ptr = (uint32_t*) pmm_alloc_page();
	memset(tab_ptr, 0, PAGE_SIZE);
	dir_ptr[0] = (uint32_t) tab_ptr | PAGE_PRESENT | PAGE_WRITE;
	// you do KNOW what those tab_ptr's stands for
	for (uint32_t i = 0; i < PG_NUM_FROM_ADDR(LOWMEM_ID_MAP_END); i++)
		tab_ptr[i] = i * 0x1000 | PAGE_PRESENT | PAGE_WRITE;

	// MOUNTING KERNEL DIRECTORY
	// mapping for directory itself
	uint32_t idx_dir = PGDIR_IDX_FROM_ADDR(KERNEL_DIR_VIRTUAL);
	uint32_t idx_tab = PGTAB_IDX_FROM_ADDR(KERNEL_DIR_VIRTUAL);
	// allocate page for directory table
	tab_ptr = (uint32_t*) pmm_alloc_page();
	memset(tab_ptr, 0, PAGE_SIZE);
	dir_ptr[idx_dir] = (uint32_t) tab_ptr | PAGE_PRESENT | PAGE_WRITE;
	// last entry of second-last table will be directory physical address
	tab_ptr[idx_tab] = (uint32_t) dir_ptr | PAGE_PRESENT | PAGE_WRITE;

	// MOUNTING KERNEL TABLES
	// last table of page directory points to directory itself
	// NOTE: after that there is no need of tables_virtual in page directort,
	// as they are accessible like linear table starting at KERNEL_TABLES_VIRTUAL
	dir_ptr[PGDIR_IDX_FROM_ADDR(KERNEL_TABLES_VIRTUAL)] = (uint32_t) dir_ptr
			| PAGE_PRESENT | PAGE_WRITE;

	// as kernel directory was mounted we can fill mountpoint info
	kernel_directory.directory = (uint32_t*) KERNEL_DIR_VIRTUAL;
	kernel_directory.pages = (uint32_t*) KERNEL_TABLES_VIRTUAL;

	// TODO: prepare tage tables for the rest of kernel address space to avoid
	// faults on switching to kernel in long ago copied directories

	for (uint32_t i = PGDIR_IDX_FROM_ADDR(KERNEL_ADDRESS_SPACE);
			i < PGDIR_IDX_FROM_ADDR(MOUNT_TABS_START); i++) {
		if (!dir_ptr[i]) {
			uint32_t* tab_ptr = (uint32_t*) pmm_alloc_page();
			memset(tab_ptr, 0, PAGE_SIZE);
			dir_ptr[i] = (uint32_t) tab_ptr | PAGE_PRESENT | PAGE_WRITE;
		}
	}

	for (uint32_t i = PGDIR_IDX_FROM_ADDR(KERNEL_ADDRESS_SPACE);
			i < PGDIR_IDX_FROM_ADDR(MOUNT_TABS_START); i++) {
		if (!dir_ptr[i]) {
			uint32_t* tab_ptr = (uint32_t*) pmm_alloc_page();
			memset(tab_ptr, 0, PAGE_SIZE);
			dir_ptr[i] = (uint32_t) tab_ptr | PAGE_PRESENT | PAGE_WRITE;
		}
	}

	// TODO: consider if this kernel mapping is necessary

	// set the current directory
	switch_page_directory(&kernel_directory);

	// enable paging
	asm volatile("mov %%cr0, %%eax;\
			orl $0x80000000, %%eax;\
			mov %%eax, %%cr0;"::);

	// faith check
	if (&kernel_directory != current_directory)
		printf("VMM: God doesn't exist.");

	// NOTE: do not mount kernel directory to itself (as it is already)

	// identity mapping pmm stack (prevent from entering THE IF in map() before
	// pmm stack will be ready)
	kernel_directory.directory[PGDIR_IDX_FROM_ADDR(PMM_STACK_START)] =
			(uint32_t) pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;
	memset(kernel_directory.pages + PG_FIRST_DIR_FROM_ADDR(PMM_STACK_START), 0
			,PAGE_SIZE);

	// NOTE: remember that every entry in directory and tables have flags,
	// to obtain addresses must be and'ed with PAGE_MASK

	// paging active
	pmm_paging_active = 1;
	can_map = true;
}

void switch_page_directory(page_directory_t* dir) {
	current_directory = dir;
	asm volatile("mov %0, %%cr3" : : "r" (dir->physical));
}

void map(uint32_t va, uint32_t pa, uint32_t flags) {
	while(can_map != true)
	{

	}
	
	uint32_t dir_entry = PGDIR_IDX_FROM_ADDR(va);
	uint32_t page_num = PG_NUM_FROM_ADDR(va);

	// find appropriate pagetable for va
	if (current_directory->directory[dir_entry] == 0) {
		
		// create pagetable holding this page
		// update current directory
		
		current_directory->directory[dir_entry] = pmm_alloc_page()
				| PAGE_PRESENT | PAGE_WRITE;
		// NEVER update kernel directory -- could screw up things
		// init page table
		
		memset(current_directory->pages + dir_entry * 1024, 0, PAGE_SIZE);
		
		}
	// //("current_directory settting\n");
	
	// NOTE: tables_virtual seems to be common over all page directories
	// page table exists, now update flags and pa
	current_directory->pages[page_num] = (pa & PAGE_ADDR_MASK) | PAGE_PRESENT
			| flags;
	
	// NOTE: if we're mapping something in kernel space from out of kernel
	// we need to be careful not to fuck up
	if (KERNEL_ADDRESS_SPACE <= va
			&& current_directory->pages[page_num]
					!= kernel_directory.pages[page_num])
					{
						printf("I THINK THIS IS BAD\n");
					}
		//("WARNING: kernel outdated\n");
}

void unmap(uint32_t va) {
	// printf("unmap address: 0x%08X\n", va);
	uint32_t page_num = PG_NUM_FROM_ADDR(va);
	// update current directory
	current_directory->pages[page_num] = 0;
	// NEVER update kernel directory -- could screw up things
	// invalidate page mapping
	asm volatile("invlpg (%0)" : : "a" (va));
}

uint8_t get_mapping(uint32_t va, uint32_t* pa) {
	uint32_t dir_entry = PGDIR_IDX_FROM_ADDR(va);
	uint32_t page_num = PG_NUM_FROM_ADDR(va);

	// Find the appropriate page table for 'va'.
	if (current_directory->directory[dir_entry] == 0)
		return 0;

	if (current_directory->pages[page_num] != 0) {
		if (pa)
			*pa = (current_directory->pages[page_num] & PAGE_ADDR_MASK)
					+ (va & PAGE_FLAGS_MASK);
		// NOTE: now it returns physical addres of variable itself, not frame
		return 1;
	}
	return 0;
}

uint32_t find_free_range(uint32_t va_start, uint32_t va_end, uint32_t size,
		uint8_t size_aligned) {
	va_start = PAGE_ROUND_DOWN(va_start);
	va_end = PAGE_ROUND_UP(va_end);
	size = PAGE_ROUND_UP(size);
	uint32_t step = (size_aligned) ? size : PAGE_SIZE;
	for (; va_start < va_end;) {
		uint32_t va;
		for (va = va_start; va < va_start + size; va += PAGE_SIZE)
			if (get_mapping(va, 0))
				break;
		if (va == va_start + size) {
			// success
			return va_start;
		} else {
			while (va_start <= va)
				va_start += step;
		}
	}
	// failed
	return va_end;
}

uint32_t allocate_range(uint32_t va_start, uint32_t va_end, uint32_t flags) {
	va_start = PAGE_ROUND_DOWN(va_start);
	va_end = PAGE_ROUND_UP(va_end);
	// allocate and map
	uint32_t pa;
	for (uint32_t va = va_start; va < va_end; va += PAGE_SIZE) {
		if (get_mapping(va, &pa)) {
			// if exists update flags
			map(va, pa, flags);
		} else {
			// if doesn't exists allocate
			map(va, pmm_alloc_page(), flags);
		}
	}
	return va_end;
}

static void find_mount_point(page_directory_mount_t* mount) {
	memset(mount, 0, sizeof(page_directory_mount_t));
	uint32_t va;
	for (va = MOUNT_DIR_START; va < MOUNT_DIR_END && !mount->directory; va +=
			PAGE_SIZE)
		if (!get_mapping(va, 0))
			mount->directory = (uint32_t*) va;
	for (va = MOUNT_TABS_START; va < MOUNT_TABS_END && !mount->pages;
			va += 1024 * PAGE_SIZE)
		if (!current_directory->directory[PGDIR_IDX_FROM_ADDR(va)])
			mount->pages = (uint32_t*) va;
	if (!mount->directory || !mount->pages)
		printf("VMM: there's no free mount points.");
}

static void mount_directory(page_directory_mount_t* mount,
		page_directory_t* dir) {
	// NOTE: these chekings are completely unnecessary since we use automatic
	// allocation of mountpoints, but leave them just in case

	// find free mount point ad assign addresses
	find_mount_point(mount);

	// some checkings
	if (NOT_PAGE_ALIGNED(mount->directory)
			|| (NOT_DIR_ENTRY_ALIGNED(mount->pages)))
		printf("VMM: mountpoint corrupted");

	// more checkings
	uint32_t dir_idx = PGDIR_IDX_FROM_ADDR(mount->pages);
	if (get_mapping((uint32_t) mount->directory, 0)
			|| current_directory->directory[dir_idx])
		printf("VMM: mountpoint occupied.");

	// mounting
	map((uint32_t) mount->directory, dir->physical, PAGE_PRESENT | PAGE_WRITE);
	current_directory->directory[dir_idx] = dir->physical | PAGE_PRESENT
			| PAGE_WRITE;
}

static void umount_directory(page_directory_mount_t* mount) {
	// NOTE: do NOT use pmm_free_page() beacuse we didn't allocate any page
	unmap((uint32_t) mount->directory);
	current_directory->directory[PGDIR_IDX_FROM_ADDR(mount->pages)] = 0;
}

extern void copy_page_physical(uint32_t* dest, uint32_t* src_dir);

page_directory_t* clone_directory(page_directory_t* src_dir) {
	// create directory
	page_directory_t* dest_dir = kmalloc(sizeof(page_directory_t));
	dest_dir->physical = pmm_alloc_page();
	// setup mount points
	page_directory_mount_t src, dest;
	mount_directory(&src, src_dir);
	mount_directory(&dest, dest_dir);
	// initialize
	memset(dest.directory, 0, PAGE_SIZE);
	// copy
	for (uint32_t i = 0; i < 1024; i++) {
		if (src.directory[i] == 0)
			continue;
		if (src.directory[i] == kernel_directory.directory[i]) {
			dest.directory[i] = src.directory[i];
		} else {
			dest.directory[i] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE
					| PAGE_USER;
			memset(dest.pages + i * 1024, 0, PAGE_SIZE);
			for (uint32_t j = 1024 * i; j < 1024 * (i + 1); j++) {
				if (src.pages[j] == 0)
					continue;
				dest.pages[j] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE
						| PAGE_USER;
				copy_page_physical((uint32_t*) dest.pages[j],
						(uint32_t*) (PAGE_ADDR_MASK & src.pages[j]));
				dest.pages[j] |= (PAGE_FLAGS_MASK & src.pages[j]);
			}
		}
	}

	// PROBLEM: what if this pagetable is linked, not copied?
	// answer: this one wouldn't be, it's not kernel one.
	// set dest directory mountpoint
	dest_dir->directory = (uint32_t*) USER_DIR_VIRTUAL;
	dest_dir->pages = (uint32_t*) USER_TABLES_VIRTUAL;

	// mapping for directory itself
	// create page table
	dest.directory[PGDIR_IDX_FROM_ADDR(dest_dir->directory)] = pmm_alloc_page()
			| PAGE_PRESENT | PAGE_WRITE;
	// make mapping
	dest.pages[PG_NUM_FROM_ADDR(dest_dir->directory)] =
			(uint32_t) dest_dir->physical | PAGE_PRESENT | PAGE_WRITE;

	// mapping for page tables (assign directory to last directory entry)
	dest.directory[PGDIR_IDX_FROM_ADDR(dest_dir->pages)] =
			(uint32_t) dest_dir->physical | PAGE_PRESENT | PAGE_WRITE;

	// unmount dest_dir directory
	umount_directory(&src);
	umount_directory(&dest);
	return dest_dir;
}

void destroy_directory(page_directory_t* dir) {
	if (dir == &kernel_directory)
		printf("VMM: deleting kernel (reference) directory.");
	page_directory_mount_t mp;
	mount_directory(&mp, dir);
	// NOTE: do not touch dir.directory and dir.pages addresses
	// first we determine protected indexes
	uint32_t dir_entry = PGDIR_IDX_FROM_ADDR(dir->pages);
	uint32_t page_num = PG_NUM_FROM_ADDR(dir->directory);
	// now free what we got to free
	uint32_t kernel_limit = PGDIR_IDX_FROM_ADDR(KERNEL_ADDRESS_SPACE);
	for (uint32_t i = 0; i < kernel_limit; i++) {
		if (mp.directory[i] == 0
				|| mp.directory[i] == kernel_directory.directory[i]
				|| i == dir_entry)
			continue;
		for (uint32_t j = 1024 * i; j < 1024 * (i + 1); j++) {
			if (mp.pages[j] == 0 || j == page_num)
				continue;
			pmm_free_page(mp.pages[j] & PAGE_ADDR_MASK);
		}
		// NOTE: do not check for page_num here deliberately to free
		// page allocated during mounting dir to itself
		pmm_free_page(mp.directory[i] & PAGE_ADDR_MASK);
	}
	pmm_free_page(dir->physical);
	umount_directory(&mp);
	kfree(dir);
}

void diff_directories(page_directory_t* adir, page_directory_t* bdir) {
	page_directory_mount_t amp, bmp;
	mount_directory(&amp, adir);
	mount_directory(&bmp, bdir);
	printf("start diff\n");
	for (uint32_t pd = 0; pd < 1024; pd++) {
		if (amp.directory[pd] != bmp.directory[pd]) {
			printf("dir: %d\n", pd);
		}
		if (amp.directory[pd] && amp.directory[pd]) {
			for (uint32_t pg = 0; pg < 1024; pg++) {
				if (amp.pages[pg] != bmp.pages[pg]) {
					printf("page: %d\n", pd * 1024 + pg);
				}
			}
		}
	}
	printf("end diff\n");
	umount_directory(&amp);
	umount_directory(&bmp);
}

void page_fault(REGISTERS *regs) {
	uint32_t cr2;
	asm volatile("mov %%cr2, %0" : "=r" (cr2));
	printf("Current directory: 0x%.8x %s\n", current_directory->physical,
			(current_directory == &kernel_directory) ? "kernel" : "");
	printf("Page fault: eip: 0x%x, faulting address: 0x%x\n", regs->eip, cr2);
	printf("Error code: %x\n", regs->err_code);
	printf("");
}
uint32_t sys_allocate_virtual_memory(uint32_t va, uint32_t size, uint32_t flags) {
    // Round up the size to the nearest page size
    size = PAGE_ROUND_UP(size);

    // Allocate physical memory
    uint32_t pa = pmm_alloc_page();
    if (pa == 0) {
        // Allocation failed
        return 0;
    }

    // Map the physical memory to virtual memory
    for (uint32_t offset = 0; offset < size; offset += PAGE_SIZE) {
        uint32_t virtual_address = va + offset;
        map(virtual_address, pa + offset, flags);
    }

    return va;
}