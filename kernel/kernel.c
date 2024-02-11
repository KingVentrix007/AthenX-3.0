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
#include "cpu.h"
#include "command.h"
#include "pageing.h"
#include "ide.h"
#include "fat_access.h"
#include "fat_filelib.h"
#include "syscall.h"
#include "vmm.h"
#include "io_ports.h"
#include "pagepmm.h"
#include "pageing.h"
#include "pmm.h"
void command_line(void);
void loop(void);
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
void loop_timer(void)
{
    // 
    
    while(1)
    {
        // sleep(1);
        // printf("Hello from the loop");
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
     idt_init();
    
    // init_vmm();
    gdt_init();
    kprints("Staring kernel\n");
    int ret = vesa_init(1024, 768, 32);
   
        if (ret < 0)
        {
            kprints("Error: vesa_init() failed\n");
        }
    init_terminal();
    init_com1();
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
        mem_main(g_kmap.available.start_addr, g_kmap.available.size);
     }
   

    
    // bios32_init();
    
    
    
   
     
    //  for(;;);
    // Print a 'X' character with color attribute 0x0F (white on black)
    print_char('X', 0x0A);
    kprints("Hello World!\nBye world");
    // void *ptr = sys_allocate_memory(KB);
    // void *ptr2 = sys_allocate_memory(KB);
    char *string1 = sys_allocate_memory(KB);
    strcpy(string1, "Hello World!");
    printf("Memory allocation output: %s\n",string1);
    // if(ptr == ptr2)
    // {
        // kprints("Memory allocation failure\n");
    // }/
    // // create_blank_pageing_dictionary();
    // // create_first_page();
    // enable();

    ata_init();
    fl_init();
    if (fl_attach_media(ide_read_sectors_fat, ide_write_sectors_fat) != FAT_INIT_OK)
    {
        printf("ERROR: Failed to init file system\n");
    }
    int num_dir;
    int num_files;
    Entry files[MAX];
    Entry dirs[MAX];
    fl_listdirectory("/",dirs,files,&num_dir,&num_files);
    mkdir("/root");
    printf("%u\n", g_kmap.available.size);
    
    InitScheduler();
   
    printf("Enabling paging\n");
    size_t size = (g_kmap.available.size/2)-10;
    printf("Size of page frame earea == %u\n", size);
    uint32_t pmm_start =  (uint32_t)g_kmap.available.start_addr+ (g_kmap.available.size/2)+5;
    // if(pmm_start == NULL)
    // {
    //     printf("We got issues with paging\n");
    // }

    printf("Num pages == %d\n",(g_kmap.available.size/2)/PAGE_SIZE);
    if(pmm_start == NULL)
    {
        printf("Couldn't allocate memory\n");
    }
    asm("cli");
    init_pmm_page(pmm_start);
    init_vmm();
    pmm_collect_pages(mboot_info);
    map_vesa();
    // kprints("Shup\n");
    // printf("Paging enabled\n");
     timer_init();
    // int x = 1/0;
    // TIMER_FUNC_ARGS timer;
    // timer.timeout = 100;
    // timer.user = TimerHandler;
    // timer_register_function(TimerHandler,&timer);
    keyboard_init();
    STI();
    // command_line();
    // CreateProcess(Process);
    CreateProcess(command_line);
    // SwitchToTask
    // // CreateProcess(loop);
    CreateProcess(loop_timer);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    // CreateProcess(Process);
    PerformButler();

    // switchTask();
    // while(1)
    // {
    //     switchTask();
    // }
        // Infinite loop
    
}
void command_line(void)
{
    // STI();
    char *input_buffer = (char *)sys_allocate_memory(KB);
    int buffer_pos = 0;
    char user[] = "Dev";
    memset(input_buffer,0,KB);
    printf("\n>");
    while(1)
    {
        // printf("c");
        char chr = (char)kb_getchar_w();
        // printf("next");
        if(chr == '\0')
        {
            continue;
        }
        if(chr != '\n')
        {
            input_buffer[buffer_pos] = chr;
            buffer_pos++;
            printf("%c",chr);
            // cmd(input_buffer);

        }
        else if (chr == '\n')
        {
           
            cmd(input_buffer);
           memset(input_buffer, 0,1024);
           buffer_pos = 0;
           printf("\n%s@%s>",user,getcwd());
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
    // TerminateProcess();
}

void loop(void)
{
    // while(1)
    // {
    //     printf("l\n");
    // }
    TerminateProcess();
}