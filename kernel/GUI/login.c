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
void set_font_bg_color_(int color);
void set_font_fg_color_(int color);
uint32_t username_start_x = 0;
uint32_t username_start_y = 0;

uint32_t password_start_x = 0;
uint32_t password_start_y = 0;




// Function prototypes for new 3D drawing styles
void draw_3d_rect(int x, int y, int width, int height, int border_color, int shadow_color, int highlight_color);

// Updated draw_login_screen function
void draw_login_screen(int screenX, int screenY) {
    // Clear the screen with the background color
    for (int y = 0; y < screenY; y++) {
        for (int x = 0; x < screenX; x++) {
            vbe_putpixel(x, y, COLOR_BG);
        }
    }

    // Title: "Login System"
    const char *title = "MILITARY LOGIN SYSTEM";
    int title_x = (screenX - (8 * strlen(title))) / 2; // Centered horizontally
    int title_y = 30; // Adjusted for smaller font size
    draw_text_login(title_x, title_y, title, COLOR_TEXT);

    // Username field
    int field_width = 8 * 15;  // 15 characters wide (8 pixels per character)
    int field_height = 16 + 8; // Font height (16 pixels) + padding
    int username_x = (screenX - field_width) / 2;
    int username_y = title_y + 50; // Spaced below the title

    draw_text_login(username_x, username_y - 20, "USERNAME:", COLOR_TEXT);
    draw_3d_rect(username_x, username_y, field_width, field_height,
                 COLOR_FIELD_BORDER, COLOR_FIELD_SHADOW, COLOR_FIELD_HIGHLIGHT);
    username_start_x = username_x+4;
    username_start_y = username_y+4;

    // Password field
    int password_x = username_x;
    int password_y = username_y + 60; // Spaced below the username field

    draw_text_login(password_x, password_y - 20, "PASSWORD:", COLOR_TEXT);
    draw_3d_rect(password_x, password_y, field_width, field_height,
                 COLOR_FIELD_BORDER, COLOR_FIELD_SHADOW, COLOR_FIELD_HIGHLIGHT);
    password_start_x = password_x+4;
    password_start_y = password_y+4;
    set_font_bg_color_(COLOR_FIELD_HIGHLIGHT);
}

// Draw a filled rectangle with 3D borders
void draw_3d_rect(int x, int y, int width, int height, int border_color, int shadow_color, int highlight_color) {
    // Draw the main rectangle
    draw_rect_login(x, y, width, height, border_color);

    // Draw shadow (bottom-right)
    for (int i = 0; i < 2; i++) {
        draw_rect_login(x + i, y + height - i, width - i, 1, shadow_color); // Bottom
        draw_rect_login(x + width - i, y + i, 1, height - i, shadow_color); // Right
    }

    // Draw highlight (top-left)
    for (int i = 0; i < 2; i++) {
        draw_rect_login(x, y + i, width - i, 1, highlight_color); // Top
        draw_rect_login(x + i, y, 1, height - i, highlight_color); // Left
    }
}

// Draw text
void draw_text_login(int x, int y, const char *text, int color) {
    while (*text) {
        draw_char(x, y, *text, color);
        x += 8; // Move to the next character position (8 pixels wide)
        text++;
    }
}

// Draw a single character at (x, y)
void draw_char(int x, int y, char c, int color) {
    set_font_fg_color_(color);
    set_font_bg_color_(COLOR_FIELD_SHADOW);
    draw_char_8x16(x, y, c);
}



// Draw a filled rectangle
void draw_rect_login(int x, int y, int width, int height, int color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            vbe_putpixel(x + j, y + i, color);
        }
    }
}

char *get_password(char *s, int n) {
        char ch;
        int count = 0;
        while (((ch = getch_()) > 0 && ch != '\n' && ch != EOF) && count < n - 1) {
            if (ch == '\b') {
                if (count > 0) { // Ensure not underflowing the buffer
                    *s = '\0';
                    s--;
                    *s = '\0';

                    // s--;
                    count--;
                    // s[count] = ' ';
                    printf("\b");
                    dbgprintf(">>%s\n",s);
                }
            } else {
                printf("*");
                *s = ch;
                s++;
                count++;
            }
        }
        int point = strlen(s);
        // printf("Point: %d\n", point);
        s[count] = '\0'; // Null-terminate the string
    return s; // Return the pointer to the string
}