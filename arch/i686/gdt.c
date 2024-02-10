/**
 * Global Descriptor Table(GDT) setup
 */
#include "gdt.h"

GDT g_gdt[NO_GDT_DESCRIPTORS];
GDT_PTR g_gdt_ptr;

/**
 * fill entries of GDT 
 */
void gdt_set_entry(int index, uint32 base, uint32 limit, uint8 access, uint8 gran) {
    GDT *this = &g_gdt[index];

    this->segment_limit = limit & 0xFFFF;
    this->base_low = base & 0xFFFF;
    this->base_middle = (base >> 16) & 0xFF;
    this->access = access;

    this->granularity = (limit >> 16) & 0x0F;
    this->granularity = this->granularity | (gran & 0xF0);

    this->base_high = (base >> 24 & 0xFF);
}

// initialize GDT
void gdt_init() {
    g_gdt_ptr.limit = sizeof(g_gdt) - 1;
    g_gdt_ptr.base_address = (uint32)g_gdt;

    // NULL segment
    gdt_set_entry(0, 0, 0, 0, 0);
    // code segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // data segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    // user code segment
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    // user data segment
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    load_gdt((uint32)&g_gdt_ptr);
}

// static void write_tss(int32_t num, uint16_t ss0, uint32_t esp0) {
// 	// Firstly, let's compute the base and limit of our entry into the GDT.
// 	uint32_t base = (uint32_t) &tss_entry;
// 	uint32_t limit = base + sizeof(tss_entry);

// 	// Now, add our TSS descriptor's address to the GDT.
// 	gdt_set_gate(num, base, limit, 0xE9, 0x00);

// 	// Ensure the descriptor is initially zero.
// 	memset(&tss_entry, 0, sizeof(tss_entry));

// 	tss_entry.ss0 = ss0; // set the kernel stack segment
// 	tss_entry.esp0 = esp0; // set the kernel stack pointer

// 	// Here we set the cs, ss, ds, es, fs and gs entries in the TSS.
// 	// These specify what segments should be loaded when the processor
// 	// switches to kernel mode. Therefore they are just our normal
// 	// kernel code/data segments - 0x08 and 0x10 respectively,
// 	// but with the last two bits set, making 0x0b and 0x13.
// 	// The setting of these bits sets the RPL (requested privilege level)
// 	// to 3, meaning that this TSS can be used to switch to
// 	// kernel mode from ring 3.
// 	tss_entry.cs = 0x0b;
// 	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs =
// 			0x13;
// }
