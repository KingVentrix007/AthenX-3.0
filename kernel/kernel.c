// Include necessary header files
#include "stdint-gcc.h"
#include "vga.h"
#include "multiboot.h"
#include "kernel.h"
#include "scanf.h"
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
#include "stdio.h"
#include "stdlib.h"
#include "immintrin.h"
#include "keyboard.h"
#include "login.h"
#include "init_options.h"
void command_line(void);
unsigned int get_random_number();
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
void cursor_flash()
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
    init(magic,addr);
    
   
}
size_t command_buffer_size = 1024;
char *input_buffer;
// char **history;
int history_pos = 0;
size_t cmd_count;
void load_history_up()
{
    if(cmd_count >= 1 && history_pos < cmd_count)
    {
        for (size_t i = 0; i < strlen(input_buffer); i++)
        {
            printf("\b");
        }
        
        memset(input_buffer,0,command_buffer_size+1);
        // strcpy(input_buffer,history[history_pos])
        printf("%s",input_buffer);

    }
    
}
void load_history_down()
{
    // printf("LOAD DPWN\n ");
    if(cmd_count >= 1 && history_pos < cmd_count)
    {
         for (size_t i = 0; i < strlen(input_buffer); i++)
        {
            printf("\b");
        }
        history_pos--;
        memset(input_buffer,0,command_buffer_size+1);
        
        printf("%s",input_buffer);

    }
}
   void draw_login_screen(int screenX, int screenY);
int gui_mode();
void command_line(void)
{
    // 
    printf("TUI MODE %d[true: %d|false: %d]\n",get_tui_state(),true,false);
    if(get_tui_state() == true)
    {
        gui_mode(get_terminal_width(),get_terminal_hight());
    }
    // register_arrow_callback(SCAN_CODE_KEY_UP,load_history_up);
    // register_arrow_callback(SCAN_CODE_KEY_DOWN,load_history_down);
    // dev_0(10);
    printf("Welcome to ");
    printf("\033[0;31m"); // Set color to red
    printf("I");
    
    // Print "r" in green
    printf("\033[0;32m"); // Set color to green
    printf("r");

    // Print "i" in yellow
    printf("\033[0;33m"); // Set color to yellow
    printf("i");

    // Print "s" in blue
    printf("\033[0;34m"); // Set color to blue
    printf("s");

    // Reset color
    printf("\033[0m");
    printf("! A simple bash like shell\nType help or iris for more information\n");

    LOG_LOCATION;
    // sleep(3);
    
    // history = (char**)malloc(1024);
    cmd_count = 0;
    input_buffer = (char *)malloc(command_buffer_size+1);
    // printf("HERE\n");
    // int buffer_pos = 0;
    char *user = malloc(1024);
    if(user == NULL)
    {
        printf("Failed to allocate memory for user buffer");
    }
    strcpy(user,"Dev");
    memset(input_buffer,0,command_buffer_size);
    // free(input_buffer);
    printf("\n%s@%d:%s>",user,get_primary_dev(),getcwd());
    while(1)
    {
        fgets(input_buffer,command_buffer_size,stdin);
        if(input_buffer == NULL || input_buffer[0] == '\0')
        {
            memset(input_buffer, 0,command_buffer_size);
            // printf("\n%s@%s>",user,getcwd());
            printf("\n%s@%d:%s>",user,get_primary_dev(),getcwd());


    

        }
        else
        {   //configuration
            // printf("input_buffer\n%s",input_buffer);
            cmd(input_buffer);
            // history[cmd_count] = (char*)malloc(strlen(input_buffer) + 1);
            cmd_count++;
            memset(input_buffer, 0,command_buffer_size);
            // printf("\n%s@%s>",user,getcwd());
            printf("\n%s@%d:%s>",user,get_primary_dev(),getcwd());

            


        }   
       
        
    }
    // TerminateProcess();
}
int gui_mode(int screen_x, int screen_y)
{
    draw_login_screen(screen_x,screen_y);
    set_terminal_postion_x(username_start_x);
    set_terminal_postion_y(username_start_y);
    while(1)
    {
        set_terminal_postion_x(username_start_x);
        set_terminal_postion_y(username_start_y);
        char username[16] = {0};
        fgets(username,15,stdin);
        set_terminal_postion_x(password_start_x);
        set_terminal_postion_y(password_start_y);
        char password[16] = {0};
        get_password(password,15);
        if(strcmp(username,"tristan") == 0 && strcmp(password,"tristan") == 0)
        {
            break;
        }

        
    }
    draw_ui_layout(screen_x,screen_y);
    set_terminal_postion_x(command_start_x);
    set_terminal_postion_y(command_start_y);
    while(1)
    {
        set_scroll_x_pos(command_start_x);
        set_terminal_postion_x(command_start_x);
        set_terminal_postion_y(command_start_y);

        char command[500] = {0};
        fgets(command,499,stdin);
        set_terminal_postion_x(command_start_x);
        for (size_t i = 0; i < strlen(command); i++)
        {
            printf(" ");
        }
        set_terminal_postion_x(output_start_x);
        set_terminal_postion_y(output_start_y);
        set_scroll_x_pos(output_start_x);
        cmd(command);
        



    }
}
int check_buffer(char *string)
{
    if(strcmp(string, "ls") == 0)
    {
        printf("\b\b");
         printf("\033[0;32m"); // Set color to green
         printf("ls");
        printf("\033[0m");
    }
}

int dev_0(int x)
{
    
    dev_2();
    int y = x;
    dev_3();

}

void dev_2()
{
    char *msg = "dev_2";
}

int dev_3()
{
    dev_4(6);
}

int dev_4(int x)
{
    // int x = 4;
    char *test = malloc(100);
    if(test == NULL)
    {
        return 0;
    }
    strcpy(test,"cat");
    printf("%s\n",test);
    int y = 2;
    int z = x/y;
    
    int out = 1/0;
//     uint32 *ret = (uint32 *)(-0x0000000);
//     memset(ret,0,100);
// }
}
