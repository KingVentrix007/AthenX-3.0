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
uint32_t command_start_x, command_start_y;
uint32_t menu_start_x, menu_start_y;
uint32_t output_start_x, output_start_y;
uint32_t score_start_x, score_start_y;
uint32_t score_width,score_hight;
#define PADDING 4
#define MENU_WIDTH 100
#define SCORE_WIDTH 200
#define COMMAND_HEIGHT 30

// Function to draw the main UI layout
void draw_ui_layout(int screenX, int screenY) {
    // Clear the screen with the background color
    for (int y = 0; y < screenY; y++) {
        for (int x = 0; x < screenX; x++) {
            vbe_putpixel(x, y, COLOR_BG);
        }
    }

    // Menu section
    menu_start_x = PADDING;
    menu_start_y = PADDING;
    int menu_height = screenY - COMMAND_HEIGHT - 3 * PADDING;
    draw_3d_rect(menu_start_x, menu_start_y, MENU_WIDTH, menu_height,
                 COLOR_FIELD_BORDER, COLOR_FIELD_SHADOW, COLOR_FIELD_HIGHLIGHT);
    draw_text_login(menu_start_x + PADDING, menu_start_y + PADDING, "MENU", COLOR_TEXT);

    // Score section
    score_start_x = screenX - SCORE_WIDTH - PADDING;
    score_start_y = PADDING;
    draw_3d_rect(score_start_x, score_start_y, SCORE_WIDTH, menu_height/2,
                 COLOR_FIELD_BORDER, COLOR_FIELD_SHADOW, COLOR_FIELD_HIGHLIGHT);
    draw_text_login(score_start_x + PADDING, score_start_y + PADDING, "SCORE", COLOR_TEXT);

    // Output (Main) section
    output_start_x = menu_start_x + MENU_WIDTH + PADDING;
    output_start_y = PADDING;
    int output_width = screenX - MENU_WIDTH - SCORE_WIDTH - 4 * PADDING;
    int output_height = menu_height;
    draw_3d_rect(output_start_x, output_start_y, output_width, output_height,
                 COLOR_FIELD_BORDER, COLOR_FIELD_SHADOW, COLOR_FIELD_HIGHLIGHT);
    // draw_text_login(output_start_x + PADDING, output_start_y + PADDING, "OUTPUT", COLOR_TEXT);
    output_start_x = menu_start_x + MENU_WIDTH + PADDING+PADDING;
    output_start_y = PADDING+PADDING;
    // Command section
    command_start_x = PADDING;
    command_start_y = screenY - COMMAND_HEIGHT - PADDING;
    draw_3d_rect(command_start_x, command_start_y, screenX - 2 * PADDING, COMMAND_HEIGHT,
                 COLOR_FIELD_BORDER, COLOR_FIELD_SHADOW, COLOR_FIELD_HIGHLIGHT);
    draw_text_login(command_start_x + PADDING+2, command_start_y + PADDING+3, "COMMAND:", COLOR_TEXT);
    command_start_y = screenY - COMMAND_HEIGHT + PADDING;
    command_start_x = PADDING+PADDING+(strlen("COMMAND>")*8)+1;

    set_font_bg_color_(COLOR_FIELD_HIGHLIGHT);
}

void fill_menu()
{

}
void fill_score()
{

}