// Include necessary header files
#include "stdint.h"
#include "vga.h"
#include "multiboot.h"
#include "kernel.h"

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
#include "kheap.h"
#include "stdbool.h"
#include "elf.h"
#include "elf_exe.h"
#include "exe.h"
#include "cursor.h"
#include "logging.h"
void command_line(void);
void loop(void);
char pch = 'A';
int generate_random_number();
static int generate_random_number_asm();
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
void loop_timer(int input)
{
    // 
    size_t ticks = get_ticks();
    size_t target = 50;
    size_t old_tick = ticks;
    while(1)
    {
        if(ticks > old_tick+target)
        {   
            update_cursor();
            old_tick = ticks;

        }
        else
        {
            ticks = get_ticks();
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

/**
 * Function Name: kmain
 * Description: Kernel main function, entry point of the operating system.
 * 
 * Parameters:
 *   magic (unsigned long) - Magic number indicating Multiboot compatibility.
 *   addr (unsigned long) - Address of Multiboot information structure.
 * 
 * Return:
 *   void
 */
void kmain(unsigned long magic, unsigned long addr)
{
    // Initialize Interrupt Descriptor Table
    idt_init();
    
    // Initialize Global Descriptor Table
    gdt_init();
    
    // Print kernel start message
    printf("Staring kernel\n");
    
    // Initialize VESA graphics mode with resolution 1024x768 and 32-bit color depth
    int ret = vesa_init(1024, 768, 32);
    if (ret < 0)
    {
        printf("Error: vesa_init() failed\n");
    }
    
    // Initialize terminal with specified resolution
    init_terminal(1024, 768);
    
    // Initialize COM1 (serial port)
    init_com1();
    char *msg = "Getting mulitboot info";
    // logging(0,__LINE__,__func__,__FILE__,"%s",msg);
    MULTIBOOT_INFO *mboot_info;
    if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
        // Print message if Multiboot magic number is detected
        printf("MULTIBOOT_BOOTLOADER_MAGIC is TRUE\n");
        printf("Getting Kernel Memory Map\n");
        
        // Cast addr to MULTIBOOT_INFO pointer
        mboot_info = (MULTIBOOT_INFO *)addr;
        
        // Clear kernel memory map structure
        memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));
        
        // Get kernel memory map
        if (get_kernel_memory_map(&g_kmap, mboot_info) < 0)
        {
            kprints("error: failed to get kernel memory map\n");
            return;
        }
        // Calculate allocation size for memory
        size_t alloc_size = (g_kmap.available.size / 2);
    }

    // Initialize ATA (IDE) interface
    ata_init();
    
    // Initialize FAT file system
    fl_init();
    if (fl_attach_media(ide_read_sectors_fat, ide_write_sectors_fat) != FAT_INIT_OK)
    {
        printf("ERROR: Failed to init file system\n");
    }
    
    // Variables for directory and file listing
    int num_dir;
    int num_files;
    Entry files[MAX];
    Entry dirs[MAX];
    
    // List root directory
    // fl_listdirectory("/", dirs, files, &num_dir, &num_files);
    // logging(0,__LINE__,__func__,__FILE__,"%s","inited fat32\n");
    
    // Create root directory
    // mkdir("/root");
    
    // Print available memory size
    // printf("%u\n", g_kmap.available.size);

    // Initialize scheduler
    InitScheduler();

    // Calculate size of page frame area
    size_t size = (g_kmap.available.size / 2) + 10;
    // printf("Size of page frame area == %u\n", size);
    
    // Calculate start address of physical memory manager
    uint32_t pmm_start = (uint32_t)g_kmap.available.start_addr;
    // Disable interrupts
    asm("cli");
    
    // Enable Physical Memory Manager
    printf("Enabling PMM\n");
    init_pmm_page(pmm_start);
    printf("PMM enabled\n");
    
    // Initialize Virtual Memory Manager
    init_vmm();
    
    // Collect pages for Physical Memory Manager
    pmm_collect_pages(mboot_info);
    
    // Map VESA memory
    map_vesa();
    init_kheap(pmm_start);
    if(KHEAP_START < __kernel_section_end)
    {
        printf("Overlap detected\n");
    }

    // char *string = kmalloc(1024);
    // strcpy(string, "hello world");
    // printf("%s\n",string);
    // Initialize timer
    // char *file_path = "/init/programs.elf";
    // int argc = 3;
    // char **argv = {"program_name", "arg1", "arg2", NULL};
    // int rand = generate_random_number();
    // printf("Random number generator output = %d\n",rand);
    // load_elf_file(file_path,argc,argv);
    timer_init();
     
    // Initialize keyboard
    keyboard_init();
    // logging(0,__LINE__,__func__,__FILE__,"%s","inited Timer\n");
    const char* filename = "/test.txt";
    FL_FILE *file_to_write = fl_fopen(filename, "w");
    FL_FILE *file_to_write_backup = file_to_write;
    // test = fl_fopen(filename, "w");
    char *test_msg = "worldfart3";
    int fl_ret_s = fl_fwrite(test_msg,1,strlen(test_msg),file_to_write);
    // printf("fl_ret_ss = %d\n", fl_ret_s);
    // printf("test_msg = %s\n", test_msg);
    // fl_fflush(test);
    // printf("test parent = %d\n", file_to_write->parentcluster);
    // printf("test data = %d\n", file_to_write->file_data_address);
    // // file_to_write->list_node
    // printf("file byte num = %d\n", file_to_write->bytenum);
    // printf("file filelength = %d\n", file_to_write->filelength);
    fl_fclose(file_to_write);
    if(file_to_write == file_to_write_backup)
    {
        printf("Writing failed kernel check\n");
    }
    printf("Reading from file %s\n", filename);
    // fl_fflush(test);
    // fclose(test);
    FILE *test2 = fl_fopen(filename, "r");
    if(test2 == NULL)
    {
        printf("Error cant read from file\n");
    }
    
    // printf("test2 parent = %d\n", test2->parentcluster);
    // printf("test2 data = %d\n", test2->file_data_address);
    // printf("file byte num = %d\n", test2->bytenum);
    // printf("file filelength = %d\n", test2->filelength);
    if(test2 == file_to_write_backup)
    {
        printf("Failed to write data\n");
    }
    char readdata[1000];
    int read_ret = fl_fread(readdata,1,50,test2);
    printf("read data = %s -> %d\n",readdata,read_ret);
    fclose(test2);
    // update_cursor_manual();
    // Print message
    // printf("Here\n");
    
    
    // Enable interrupts
    STI();
    
    // Create command line process
    CreateProcess(command_line);
    // CreateProcess(update_cursor);

    // execute_file(file_path,argc,argv);
    // Create loop timer process
    CreateProcess(loop_timer);
    
    // Perform Butler routine
    PerformButler();
}

void command_line(void)
{
    // STI();
    // printf("Commnd command\n================================\n");
    char *input_buffer = (char *)kmalloc(1024);
    // printf("HERE\n");
    int buffer_pos = 0;
    char user[] = "Dev";
    memset(input_buffer,0,1024);
    printf("\n>");
    while(1)
    {
        // printf("c");
        char chr = (char)get_char(0);
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
        else if(chr == '\b')
        {
            input_buffer[buffer_pos] = "\0";
            buffer_pos--;
            printf("%c",chr);
        }
        else if (chr == '\n')
        {
           
            cmd(input_buffer);
           memset(input_buffer, 0,1024);
           buffer_pos = 0;
           printf("\n%s@%s>",user,"M");
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
static int generate_random_number_asm() {
    int result;
    // Execute RDRAND instruction
    asm volatile("rdrand %0" : "=r" (result));
    return result;
}

int generate_random_number() {
    int random_number;
    do {
        random_number = generate_random_number_asm();
    } while (random_number == 0); // Retry if RDRAND returns 0 (failure)
    return random_number;
}