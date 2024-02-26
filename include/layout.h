#ifndef LAYOUT_H
#define LAYOUT_H

/**
 * Almost all of these values _MUST_ be page aligned.
 * Boundary between kernel and user data _MUST_ be also page directory entry
 * (1024*0x1000 = 0x400000) aligned
 */

#define LOWMEM_ID_MAP_START		0x00000000	// == 0x00000000
#define VIDEO_MEM				0x000B8000	// == 0x000B8000
#define LOWMEM_ID_MAP_END		0x00400000	// <= 0x00400000

#define USER_TEXT_START			0x10000000	// defined in user.ld

#define USER_DIR_VIRTUAL		0xEFBFF000
#define USER_TABLES_VIRTUAL		0xEFC00000

#define USER_STACKS_START		0xE0000000
#define USER_STACKS_END			0xF0000000

// from this point (KERNEL_ADDRESS_SPACE) starts kernel part of memory
#define KERNEL_ADDRESS_SPACE	0xF0000000

#define KHEAP_START				0xF0000000
#define KHEAP_END				0xFD000000

#define MOUNT_TABS_START		0xFD000000
#define MOUNT_TABS_END			0xFF000000

// no effect as currently kernel resides in low mem
#define KERNEL_HIGHMEM			0xFF000000
#define KERNEL_HIGHMEM_LIMIT	0xFF7FB000

#define MOUNT_DIR_START			0xFF7FB000
#define MOUNT_DIR_END			0xFF7FF000

#define PMM_STACK_START			0xFF7FF000
#define PMM_STACK_END			0xFFBFF000

#define KERNEL_DIR_VIRTUAL		0xFFBFF000
#define KERNEL_TABLES_VIRTUAL	0xFFC00000
#define	KERNBASE	0xffffffff80000000
#endif // LAYOUT_H