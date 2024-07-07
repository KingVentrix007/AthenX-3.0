
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
#include "termianl.h"
#include "clock.h"
#include "printf.h"
#include "rsdp.h"
#include "acpi.h"
#include "vga.h"
#include "idt.h"
#include "gdt.h"
#include "debug_term.h"
#include "string.h"
#include "pmm.h"
#include "vmm.h"
#include "pci_dev.h"
#include "timer.h"
#include "keyboard.h"
#include "stdlib.h"
#include "kheap.h"
#include "scheduler.h"
#include "dwarf.h"
#include <stdio.h>
#include "ini/ini.h"
#include "inttypes.h"
#include "ahci.h"
#include "vfs.h"
#include "system.h"
MULTIBOOT_INFO *mboot_info;
struct BootConfig {
    char version_number[20];
    char program_path[256];
    char bin_path[256];
    char error_log[256];
    int default_drive;
    bool verbose;
};
typedef struct {
    uint32 mod_start;
    uint32 mod_end;
    uint32 string;
    uint32 reserved;
} MULTIBOOT_MODULE;
void store_modules(MULTIBOOT_INFO *mboot_info, MULTIBOOT_MODULE *stored_modules) {
    if (mboot_info->flags & (1 << 3)) { // Check if the modules are present
        MULTIBOOT_MODULE *modules = (MULTIBOOT_MODULE *)mboot_info->modules_addr;
        for (uint32 i = 0; i < mboot_info->modules_count; i++) {
            stored_modules[i].mod_start = modules[i].mod_start;
            stored_modules[i].mod_end = modules[i].mod_end;
            stored_modules[i].string = modules[i].string;
            printf("Module %d is %s\n", i,(char *)modules[i].string);
            stored_modules[i].reserved = modules[i].reserved;
        }
    } else {
        // No modules are present
        printf("No modules are present\n");
    }
}
void print_ram_size(uint64_t ram_size_kb) {
    // Determine the color based on RAM size
    const char *color_code;
    char *ram_size_str = formatBytes(ram_size_kb*1024);
    if (ram_size_kb <= 1048576) {
        // Red (Critical): 1GB or less (since 1 GB = 1024 MB = 1048576 KB)
        color_code = "\x1b[31m"; // ANSI escape code for red
        printf("-\tRAM size: ");
        printf("%s",color_code);
        printf("%u Kilobytes(", ram_size_kb);

    } else if (ram_size_kb <= 4194304) {
        // Yellow (Caution): 2GB to 4GB (2048 MB to 4096 MB)
        color_code = "\x1b[33m"; // ANSI escape code for yellow
        printf("-\tRAM size: ");
        printf("%s",color_code);
        printf("%u Kilobytes(", ram_size_kb);
        

    } else if (ram_size_kb <= 8388608) {
        // Green (Good): 4GB to 8GB (4096 MB to 8192 MB)
        color_code = "\x1b[32m"; // ANSI escape code for green
        printf("-\tRAM size: ");
        printf("%s",color_code);
        printf("%u Kilobytes(", ram_size_kb);

    } else {
        // Blue (Excellent): More than 8GB (more than 8192 MB)
        color_code = "\x1b[34m"; // ANSI escape code for blue
        printf("-\tRAM size: ");
        printf("%s",color_code);
        printf("%u Kilobytes(", ram_size_kb);

    }
    printf("%s+-", ram_size_str);
    printf(")\n");
    printf("\033[32m");
    // char *ram_size_str = formatBytes(ram_size_kb*1024);
    // Print RAM size with color
    // printf("%s-\tRAM size: %u Kilobytes\x1b[0m\n", color_code, ram_size_kb);
}
void write_first_module_to_file(MULTIBOOT_INFO *mboot_info) {
    // Ensure there is at least one module
    if (mboot_info->modules_count < 1) {
        printf("No modules found.\n");
        return;
    }

    // Get the first module
    multiboot_module_t *module = (multiboot_module_t *)mboot_info->modules_addr;

    // Calculate module size
    size_t module_size = module->end - module->start;

    // Open a file for writing
    FILE *file = fl_fopen("/install.iso", "wb");
    if (file == NULL) {
        printf("Error opening file");
        return;
    }

    // Write module data to the file
    fl_fseek(file, 0, SEEK_SET); // Ensure we start at the beginning of the file
    fl_fwrite((void *)module->start, 1, module_size, file);

    // Close the file
    fl_fclose(file);

    printf("Module written to install.iso successfully.\n");
}
int fill_program_list(int num_programs,Entry *entries);
/**
 * Function Name: init
 * Description: Initializes the kernel and system components.
 *
 * Parameters:
 *   magic (unsigned long) - Magic number passed by bootloader.
 *   addr (unsigned long) - Address of multiboot information structure.
 *
 * Return:
 *   void
 */
void init(unsigned long magic, unsigned long addr) {
    // Initialize Interrupt Descriptor Table
    idt_init();

    // Initialize Global Descriptor Table
    gdt_init();
    init_com1();
    timer_init();
    
    // Print kernel start message
    printf("\033[1;34mStarting kernel\n"); // Set text color to blue
    printf("Hello World!\033[0m\n"); // Reset text color to default

    // Initialize VESA graphics mode
    int width = 1024;
    int height = 768;
    int ret = vesa_init(width, height, 32);
    printf_com("Ret = %d\n", ret);
    if (ret < 0) {
        // Try different resolutions if initialization fails
        width = 800;
        height = 600;
        ret = vesa_init(width, height, 32);
        printf_com("-\tScreen resolution: %dx%d\n", width, height);
        if (ret < 0) {
            width = 720;
            height = 480;
            ret = vesa_init(width, height, 32);
            if (ret < 0) {
                printf_com("\n\n\033[1;31mPrinting all modes:\n\n"); // Set text color to red
                vbe_print_available_modes();
            }
        }
    }

    // Initialize terminal with specified resolution
    init_terminal(width, height);
    init_debug_terminal(width, height);

    
    printf_com("Made it here\n");
    int acpi = initAcpi();
    int acpi2 = acpiEnable();
    if(acpi2 != acpi)
    {
        if(acpi2 < acpi)
        {
            acpi = acpi2;
        }
        else if (acpi < acpi2)
        {
            acpi = acpi;
        }
        {
            /* code */
        }
        
    }

    // Print system information
    printf("\033[33mCurrent Date and Time:\n"); // Set text color to yellow
    print_date();
    printf("\033[0m");
    printf("   ");
    printf("\033[32mBooting AthenX-3.0\n"); // Set text color to green
    printf("\033[0m");
    printf("Version: %s - %d.%d.%d\n", VERSION_STRING, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("Compile version %d\n", VERSION_COMPILE);
    printf("Please stand by\n");
    printf("Booting: \n");
    int draw_x = get_terminal_postion_x();
    int draw_y = get_terminal_postion_y();
    set_terminal_postion_x(draw_x + 120); // Adjust position for loading bar

    // Display loading bar
    int total_steps = 11;
    int current_step = 1;
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);
    // printf("\n\nLoading multiboot info\n");
    MULTIBOOT_INFO *mboot_info;
    char grub_command_line[256];
    struct BootConfig config;
    char *ini_data;
    if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
        // Cast addr to MULTIBOOT_INFO pointer
        mboot_info = (MULTIBOOT_INFO *)addr;

        // Clear kernel memory map structure
        memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));

        // Get kernel memory map
        if (get_kernel_memory_map(&g_kmap, mboot_info) < 0) {
            printf("\033[1;31merror: failed to get kernel memory map\n"); // Set text color to red
            return -1;
        }
       printf_t("\nThere are %d grub modules\n", mboot_info->modules_count);
        
        strcpy(grub_command_line,mboot_info->cmdline);
        // printf("Grub cmd == %s\n",);
        multiboot_module_t *module = (multiboot_module_t *)mboot_info->modules_addr;
        init_system_info();
        ini_data = module[0].start;
        // printf("Ini data == %s\n", ini_data);
        
        for (int i = 0; i < mboot_info->modules_count; ++i) {
            // printf("Module %d(%s): Start Address %p, Size %d bytes\n", i + 1,  module[i].name,(void *)module[i].start,module[i].end - module[i].start);
            // printf("Module == %s",(unsigned char *)(module[i].start));
            // Example: Printing the first few bytes of the module as hexadecimal
            for (int j = 0; j < 16 && j < (module[i].end - module[i].start); ++j) {
                // printf("%0x ", *((unsigned char *)(module[i].start + j)));
            }
            printf("\n");
        }
        // Calculate allocation size for memory
    }
    load_boot_config(ini_data, &config);
    if(config.verbose != true)
    {
        disable_verbose();
    }
    printf_t("Initializing com port 1\n");
    init_com1();
    
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);

    // Initialize Scheduler
    printf_t("Initializing scheduler\n");
    InitScheduler();
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);

    // Initialize Physical Memory Manager
    size_t size = (g_kmap.available.size / 2) + 10;
    uint32_t pmm_start = (uint32_t)g_kmap.available.start_addr;
    asm("cli");
    printf_t("Initiating physical memory manager\n");
    init_pmm_page(pmm_start, g_kmap.available.size);
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);
    printf_t("Initializing Virtual Memory Manager\n");
    // Initialize Virtual Memory Manager
    init_vmm();
    LOG_LOCATION;

    // Collect pages for Physical Memory Manager
    pmm_collect_pages(mboot_info);

    // Map VESA memory
    map_vesa();
    LOG_LOCATION;

    // Initialize kernel heap
    printf_t("Initializing kernel heap\n");
    init_kheap(g_kmap.available.size);
    
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);
    printf_t("Scanning PCI\n");
    pci_scan();
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);
    printf_t("Attempting to initialize AHCI driver\n");
    init_storage();
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);
    // Initialize FAT file system
    printf_t("Init keyboard\n");
    keyboard_init();
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);
    STI();
    printf_t("Initialize FAT file system\n");

    int fs_init = init_file_system(config.default_drive);
    if(fs_init == -1)
    {
        printf("Failed to init filesystem for drive %d\n",config.default_drive);
    }
    CLI();
    LOG_LOCATION;
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);
    

    // Initialize timer
    // printf("Initializing timer\n");
    timer_init();
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;
    

    int ret_buf = vesa_init_buffers();
    printf_com("%d\n", ret_buf);
    char *test_malloc = malloc(1024 * 1024 * 1024);
    if (test_malloc == NULL) {
        printf("\033[1;31mCouldn't allocate test memory\n"); // Set text color to red
    } else {
        printf_debug("\033[1;32mSuccessfully allocated memory of 1GB\n"); // Set text color to green
    }
    LOG_LOCATION;

    // Free allocated memory
    const char *test_b = "This allocated memory";
    strcpy(test_malloc, test_b);
    if (strcmp(test_b, test_malloc) != 0) {
        printf("\033[1;31mMapping of test allocation may have failed\n"); // Set text color to red
    }
    free(test_malloc);
    draw_loading_bar(++current_step, total_steps, draw_x, draw_y, VBE_RGB(255, 0, 0), 2);

    LOG_LOCATION;

    

    int num_programs;
    int num_program_dirs;
    fl_count_files(config.program_path, &num_program_dirs, &num_programs);
    Entry *programs = malloc(sizeof(Entry) * num_programs);
    int num_programs_check;
    fl_populate_file_list(config.program_path, programs, &num_programs_check);
    if (num_programs_check != num_programs) {
        printf("\033[1;31mError getting list of programs\n"); // Set text color to red
    } else {
        fill_program_list(num_programs, programs);
    }
    printf("Loaded %d programs\n", num_programs);
    

    printf("\033[1;32mSystem Initialization Complete\n"); // Set text color to green
    printf("System Info:\n");

    uint64_t ram_size = mboot_info->mem_low + mboot_info->mem_high;
    // printf("%u bytes",ram_size); //3145215
    if (ram_size <= 1048063)
    {
        printf("ERROR: Inadequate memory. This OS requires more than 1GB of RAM to run.\n");
        printf("Please increase memory and try again.\n");
        printf("The system will now hang.\n");
        for (;;); // Infinite loop to halt the system
    }
    // char *ram_size_str = formatBytes64(ram_size);
    print_ram_size(ram_size);
    char *allocation_heap = formatBytes(g_kmap.available.size);
    printf("-\tAllocation heap size: %u Kilobytes (%s)\n", g_kmap.available.size / 1024,allocation_heap);

    char cpu_name[49];
    char architecture[5];
    unsigned int family, model, stepping;
    get_cpu_info(cpu_name, architecture, &family, &model, &stepping);
    //  printf("CPU Manufacturer: %s\n", sys_info.cpu_info.manufacturer);
    // printf("CPU Model: %s\n", sys_info.cpu_info.model_name);
    // printf("CPU Cores: %d\n", sys_info.cpu_info.cores);
    // printf("CPU Frequency: %u MHz\n", sys_info.cpu_info.frequency);
    printf("-\tCPU Name: %s\n", cpu_name);
    printf("-\tArchitecture: %s\n", architecture);
    printf("-\tFamily: %d, Model: %d, Stepping: %d\n", family, model, stepping);
    printf("-\tScreen resolution: %dx%d\n", width, height);

    // const char *acpi_status = (acpi == 0) ? "true" : "false";
    if(acpi != 0)
    {
        
    }
    // const char *acpi_status = (acpi == 0) ? "\x1b[32mtrue\x1b[0m":"\x1b[31mFALSE\x1b[0m";
    // printf("-\tACPI enabled: %s\n", acpi_status);
    const char *acpi_status2 = (acpi == 0) ? "\x1b[34mtrue\x1b[0m" : "\x1b[31mFALSE\x1b[0m";
    printf("-\tACPI enabled: %s\n", acpi_status2);

    printf("\033[0m"); // Reset text formatting and colors

    printf("Device Info:\n");
    print_pci_devices();
    printf("Available drives\n");
    list_devices();

    

    printf("Starting shell\n");
    if(strcmp(grub_command_line,"install")==0)
    {   
        printf("Installing\n");
        write_first_module_to_file(mboot_info);
        install_athenx();

    }
    
    CreateProcess(command_line);
    CreateProcess(cursor_flash);
    PerformButler();

    STI();
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
        printf("\n\n");
        executables_path[i] = (char*)malloc(strlen(entries[i].name));
        strcpy(executables_path[i],entries[i].name);
        
        if (executables_path[i] == NULL) {
            perror("Memory allocation failed");
            return -1;
        }
    }
    
}

int print_pci_devices()
{
    // Print the number of devices for each device type if there are more than 0 devices
    if (get_num_unclassified_devices() > 0) {
        printf("-\tNumber of Unclassified Devices: %d\n", get_num_unclassified_devices());
    }
    if (get_num_mass_storage_devices() > 0) {
        printf("-\tNumber of Mass Storage Devices: %d\n", get_num_mass_storage_devices());
    }
    if (get_num_network_devices() > 0) {
        printf("-\tNumber of Network Devices: %d\n", get_num_network_devices());
    }
    if (get_num_display_devices() > 0) {
        printf("-\tNumber of Display Devices: %d\n", get_num_display_devices());
    }
    if (get_num_multimedia_devices() > 0) {
        printf("-\tNumber of Multimedia Devices: %d\n", get_num_multimedia_devices());
    }
    if (get_num_memory_devices() > 0) {
        printf("-\tNumber of Memory Devices: %d\n", get_num_memory_devices());
    }
    if (get_num_bridge_devices() > 0) {
        printf("-\tNumber of Bridge Devices: %d\n", get_num_bridge_devices());
    }
    if (get_num_simple_comm_devices() > 0) {
        printf("-\tNumber of Simple Communications Devices: %d\n", get_num_simple_comm_devices());
    }
    if (get_num_base_system_periph_devices() > 0) {
        printf("-\tNumber of Base System Peripheral Devices: %d\n", get_num_base_system_periph_devices());
    }
    if (get_num_input_devices() > 0) {
        printf("-\tNumber of Input Devices: %d\n", get_num_input_devices());
    }
    if (get_num_docking_devices() > 0) {
        printf("-\tNumber of Docking Devices: %d\n", get_num_docking_devices());
    }
    if (get_num_processor_devices() > 0) {
        printf("-\tNumber of Processor Devices: %d\n", get_num_processor_devices());
    }
    if (get_num_serial_bus_devices() > 0) {
        printf("-\tNumber of Serial Bus Devices: %d\n", get_num_serial_bus_devices());
    }
    if (get_num_wireless_devices() > 0) {
        printf("-\tNumber of Wireless Devices: %d\n", get_num_wireless_devices());
    }
    if (get_num_intelligent_io_devices() > 0) {
        printf("-\tNumber of Intelligent IO Devices: %d\n", get_num_intelligent_io_devices());
    }
    if (get_num_satellite_devices() > 0) {
        printf("-\tNumber of Satellite Communication Devices: %d\n", get_num_satellite_devices());
    }
    if (get_num_encryption_devices() > 0) {
        printf("-\tNumber of Encryption Devices: %d\n", get_num_encryption_devices());
    }
    if (get_num_signal_processing_devices() > 0) {
        printf("-\tNumber of Signal Processing Devices: %d\n", get_num_signal_processing_devices());
    }
    if (get_num_processing_accel_devices() > 0) {
        printf("-\tNumber of Processing Accelerator Devices: %d\n", get_num_processing_accel_devices());
    }
    if (get_num_non_essential_devices() > 0) {
        printf("-\tNumber of Non-Essential Instrumentation Devices: %d\n", get_num_non_essential_devices());
    }
    if (get_num_coprocessor_devices() > 0) {
        printf("-\tNumber of Coprocessor Devices: %d\n", get_num_coprocessor_devices());
    }
    return 0;
}
// boot_config.c

// Structure to hold boot configuration data

// Callback function to handle parsed INI file data
static int config_handler(void *user, const char *section, const char *name,
                          const char *value) {
    struct BootConfig *config = (struct BootConfig *)user;

    if (strcmp(section, "version") == 0) {
        if (strcmp(name, "version_number") == 0) {
            strncpy(config->version_number, value, sizeof(config->version_number));
        }
    } else if (strcmp(section, "paths") == 0) {
        if (strcmp(name, "program_path") == 0) {
            strncpy(config->program_path, value, sizeof(config->program_path));
        } else if (strcmp(name, "bin_path") == 0) {
            strncpy(config->bin_path, value, sizeof(config->bin_path));
        }
    } else if (strcmp(section, "logs") == 0) {
        if (strcmp(name, "error_log") == 0) {
            strncpy(config->error_log, value, sizeof(config->error_log));
        }
    } else if (strcmp(section, "Settings") == 0) {
        if (strcmp(name, "VerboseOutput") == 0) {
            config->verbose = (strcmp(value, "true") == 0);
        } else if (strcmp(name, "DefaultBootDrive") == 0) {
            config->default_drive = atoi(value);
        }
    }

    return 1; // Continue parsing
}

int load_boot_config(const char *data,struct BootConfig *boot_config) {
    struct BootConfig config;

    // Initialize config with default values
    memset(boot_config, 0, sizeof(struct BootConfig));

    // Parse the INI file
    int ret = ini_parse(data, config_handler, boot_config);
    if ( ret < 0) {
        printf("Error: Can't load ini file %d\n",ret);
        return -1;
    }

    // Now, 'config' contains the parsed boot configuration data
    // printf("AthenX Version: %s\n", boot_config->version_number);
    // printf("Program Path: %s\n", boot_config->program_path);
    // printf("Bin Path: %s\n", boot_config->bin_path);
    // printf("Error Log: %s\n",boot_config->error_log);
    // // boot_config = &config;
    // You can use the parsed data as needed for booting AthenX

    return 0;
}
