// Include necessary header files
#include <stdint.h>
#include "vga.h"
#include "multiboot.h"
#include "kernel.h"
#include "mem.h"
#include "idt.h"
#include "gdt.h"
#include "printf.h"
#include "vesa.h"
#include "termianl.h"
#include "kernel.h"
#include "timer.h"
#include "scheduler.h"
void kernel_entry();
void multi1(void);
void multi2(void);
void multi3(void);
char pch = 'A';
void Process(void)
{
    char ch = pch ++;
    while (true) {
        printf("%c",ch);
        sleep(500 + (ch * 100));

        if ((ch % 2) == 0) {
            printf("%c",ch);
            printf('!');
            TerminateProcess();
        }
    }
}

KERNEL_MEMORY_MAP g_kmap;
int get_kernel_memory_map(KERNEL_MEMORY_MAP *kmap, MULTIBOOT_INFO *mboot_info) {
    uint32 i;

    if (kmap == NULL) return -1;
    kmap->kernel.k_start_addr = (uint32)&__kernel_section_start;
    kmap->kernel.k_end_addr = (uint32)&__kernel_section_end;
    kmap->kernel.k_len = ((uint32)&__kernel_section_end - (uint32)&__kernel_section_start);

    kmap->kernel.text_start_addr = (uint32)&__kernel_text_section_start;
    kmap->kernel.text_end_addr = (uint32)&__kernel_text_section_end;
    kmap->kernel.text_len = ((uint32)&__kernel_text_section_end - (uint32)&__kernel_text_section_start);

    kmap->kernel.data_start_addr = (uint32)&__kernel_data_section_start;
    kmap->kernel.data_end_addr = (uint32)&__kernel_data_section_end;
    kmap->kernel.data_len = ((uint32)&__kernel_data_section_end - (uint32)&__kernel_data_section_start);

    kmap->kernel.rodata_start_addr = (uint32)&__kernel_rodata_section_start;
    kmap->kernel.rodata_end_addr = (uint32)&__kernel_rodata_section_end;
    kmap->kernel.rodata_len = ((uint32)&__kernel_rodata_section_end - (uint32)&__kernel_rodata_section_start);

    kmap->kernel.bss_start_addr = (uint32)&__kernel_bss_section_start;
    kmap->kernel.bss_end_addr = (uint32)&__kernel_bss_section_end;
    kmap->kernel.bss_len = ((uint32)&__kernel_bss_section_end - (uint32)&__kernel_bss_section_start);

    kmap->system.total_memory = mboot_info->mem_low + mboot_info->mem_high;

    for (i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP)) {
        MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)(mboot_info->mmap_addr + i);
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        // make sure kernel is loaded at 0x100000 by bootloader(see linker.ld)
        if (mmap->addr_low == kmap->kernel.text_start_addr) {
            // set available memory starting from end of our kernel, leaving 1MB size for functions exceution
            kmap->available.start_addr = kmap->kernel.k_end_addr + 1024 * 1024;
            kmap->available.end_addr = mmap->addr_low + mmap->len_low;
            // get availabel memory in bytes
            kmap->available.size = kmap->available.end_addr - kmap->available.start_addr;
            return 0;
        }
    }

    return -1;
}

// Kernel entry point
void kmain(unsigned long magic, unsigned long addr)
{
    gdt_init();
    idt_init();
    // bios32_init();
    kprints("Staring kernel\n");
    int ret = vesa_init(1024, 768, 32);
   
    keyboard_init();
        if (ret < 0)
        {
            kprints("Error: vesa_init() failed\n");
        }
    init_terminal();
    InitScheduler();
     timer_init();
    MULTIBOOT_INFO *mboot_info;
     if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
     {
        kprints("MULTIBOOT_BOOTLOADER_MAGIC is TRUE\n");
        kprints("Getting Kernel Memory Map\n");
        mboot_info = (MULTIBOOT_INFO *)addr;
        memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));
        if (get_kernel_memory_map(&g_kmap, mboot_info) < 0) {
            kprints("error: failed to get kernel memory map\n");
            return;
        }
        init_memory_allocation(g_kmap.available.start_addr, g_kmap.available.size);
     }
    // Print a 'X' character with color attribute 0x0F (white on black)
    print_char('X', 0x0A);
    kprints("Hello World!\nBye world");
    void *ptr = sys_allocate_memory(KB);
    void *ptr2 = sys_allocate_memory(KB);
    char *string1 = sys_allocate_memory(KB);
    strcpy(string1, "Hello World!");
    printf("Memory allocation output: %s\n",string1);
    if(ptr == ptr2)
    {
        kprints("Memory allocation failure\n");
    }
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
    CreateProcess(Process);
     PerformButler();
    // switchTask();
    // while(1)
    // {
    //     switchTask();
    // }
        // Infinite loop
    char *input_buffer = (char *)sys_allocate_memory(KB);
    int buffer_pos = 0;
    memset(input_buffer,0,KB);
    while(1==1)
    {

        char chr = (char)kb_getchar_w();
        if(chr != '\n')
        {
            input_buffer[buffer_pos] = chr;
            buffer_pos++;
            printf("%c",chr);
           
        }
        else if (chr == '\n')
        {
           printf("\nOuput: %s\n",input_buffer);
           memset(input_buffer, 0,1024);
           buffer_pos = 0;
        }
        // if(buffer_pos >= get_memory_size(input_buffer)-10)
        // {
        //     printf("Resizing input buffer\n");
        //     input_buffer = sys_reallocate_memory(input_buffer, get_memory_size(input_buffer),get_memory_size(input_buffer)+KB);
        // }
        // else
        // {
        //     pr
        // }
        
    }
}
// Example task functions
void multi1(void) {
    printf("Task Multi1 running!\n");
    
}

void multi2(void) {
    printf("Task Multi2 running!\n");
   
}

void multi3(void) {
    printf("Task Multi3 running!\n");
    
}
