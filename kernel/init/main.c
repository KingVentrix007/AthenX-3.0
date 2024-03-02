
#include "ide.h"
#include "fat_filelib.h"
#include "multiboot.h"
#include "kernel.h"
#include "layout.h"
#include "stdint.h"
#include "vesa.h"
#include "vmm.h"
#include "pmm.h"
#include "io_ports.h"
// #include "stb_images.h"
#include "img.h"
#include "command.h"
#include "errno.h"
#include "../info.c"
int fill_program_list(int num_programs,Entry *entries);
/**
 * Function Name: init
 * Description: Initializes various system components during boot-up.
 *
 * Parameters:
 *   magic (unsigned long) - Multiboot magic number
 *   addr (unsigned long) - Address of Multiboot information
 *
 * Return:
 *   void
 */
void init(unsigned long magic, unsigned long addr) {
    kprints("");

    // Initialize Interrupt Descriptor Table
    idt_init();

    // Initialize Global Descriptor Table
    gdt_init();

    int total_steps = 10; // Total number of steps for the loading bar
    int current_step = 0; // Current step of the loading process

    // Print kernel start message
    kprints("Staring kernel\n");

    // Initialize VESA graphics mode
    int ret = vesa_init(1024, 768, 32);
    if (ret < 0) {
        //printf("Error: vesa_init() failed\n");
    }
  
    LOG_LOCATION;
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    // Initialize terminal with specified resolution
    init_terminal(1024, 768);
    //   printf("================================================================");
    printf("Booting AthenX-3.0\n");
    printf("Verion: %s - %d.%d.%d\n",VERSION_STRING,VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH);
    printf("Compile version %d\n",VERSION_COMPILE);
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;

    // Initialize COM1 (serial port)
    init_com1();
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;

    // Other initialization code...

    MULTIBOOT_INFO *mboot_info;
    if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
        // Cast addr to MULTIBOOT_INFO pointer
        mboot_info = (MULTIBOOT_INFO *)addr;

        // Clear kernel memory map structure
        memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));

        // Get kernel memory map
        if (get_kernel_memory_map(&g_kmap, mboot_info) < 0) {
            kprints("error: failed to get kernel memory map\n");
            return;
        }
        // Calculate allocation size for memory
        size_t alloc_size = (g_kmap.available.size / 2);
    }
    LOG_LOCATION;

    // Initialize ATA drivers
    ata_init();
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;

    // Initialize FAT file system
    fl_init();
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    if (fl_attach_media(ide_read_sectors_fat, ide_write_sectors_fat) != FAT_INIT_OK) {
        printf("ERROR: Failed to init file system\n");
    }
    LOG_LOCATION;

    // Initialize Scheduler
    InitScheduler();
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);
    
    LOG_LOCATION;

    // Calculate size of page frame area
    size_t size = (g_kmap.available.size / 2) + 10;

    // Calculate start address of physical memory manager
    uint32_t pmm_start = (uint32_t)g_kmap.available.start_addr;
    // Disable interrupts
    asm("cli");

    // Enable Physical Memory Manager
    init_pmm_page(pmm_start, g_kmap.available.size);
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    // Initialize Virtual Memory Manager
    init_vmm();
    LOG_LOCATION;

    // Collect pages for Physical Memory Manager
    pmm_collect_pages(mboot_info);

    // Map VESA memory
    map_vesa();
    LOG_LOCATION;

    // Initialize kernel heap
    init_kheap(g_kmap.available.size);
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    if (KHEAP_START < __kernel_section_end) {
        //printf("Overlap detected\n");
    }
    LOG_LOCATION;
    
    char *path = "/img/FlyingOwl.png";
    // int reti = draw_img(path,10,10);
    // printf("ret = %d\n",reti);
    LOG_LOCATION;

    // Scan PCI devices
    pci_scan();
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;

    // Initialize timer
    timer_init();
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;

    // Initialize keyboard
    keyboard_init();
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;

    char *test_malloc = malloc(1024 * 1024 * 1024);
    if (test_malloc == NULL) {
        printf("Couldn't allocate test memory\n");
    } else {
        //printf("Got memory of size 1GB\n");
    }
    LOG_LOCATION;

    // Free allocated memory
    char *test_b = "This allocated memory";
    strcpy(test_malloc, test_b);
    if(strcmp(test_b,test_malloc)!= 0)
    {
        printf("Mapping of test allocation may have failed\n");
    }
    //printf("%s", test_malloc);
    free(test_malloc);
    draw_loading_bar(++current_step, total_steps, 509, 100, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;
    int num_programs;
    int num_program_dirs;
    fl_count_files("/bin/",&num_program_dirs,&num_programs);
    Entry *programs = malloc(sizeof(Entry)*num_programs);
    int num_programs_check;
    fl_populate_file_list("/bin/",programs,&num_programs_check);
    if(num_programs_check != num_programs)
    {
        printf("Error getting list of programs\n");
    }
    else
    {
        fill_program_list(num_programs,programs);

    }
    printf("System Initialization Complete\n");
}

int fill_program_list(int num_programs,Entry *entries)
{
    executables_path= (char**)malloc(num_programs * sizeof(char*));
      if (executables_path == NULL) {
        perror("Memory allocation failed");
        return -1;
    }
    for (size_t i = 0; i < num_programs; i++)
    {
        executables_path[i] = (char*)malloc(strlen(entries[i].name));
        strcpy(executables_path[i],entries[i].name);
        printf("Executable %s found\n",executables_path[i]);
        if (executables_path[i] == NULL) {
            perror("Memory allocation failed");
            return -1;
        }
    }
    
}