#include "string.h"
#include "vga.h"
#include <stddef.h>
#define VGA_WIDTH 80
int vga_pos_x = 0;
int vga_pos_y = 0;
void print_char_at(char character, uint8_t color, int x, int y)
{
    // VGA text mode memory address
    volatile uint16_t* video_memory = (uint16_t*)0xB8000;

    // Calculate the screen offset based on x and y
    size_t offset = y * VGA_WIDTH + x;

    // Combine character and color attribute
    uint16_t entry = (color << 8) | character;

    // Write to the specified position on the screen
    video_memory[offset] = entry;
}
void print_char(char character, uint8_t color)
{
    // VGA text mode memory address
    volatile uint16_t* video_memory = (uint16_t*)0xB8000;
    
    // Combine character and color attribute
    uint16_t entry = (color << 8) | character;

    // Write to the screen
    *video_memory = entry;
}
void kprints(char *string)
{
    int len = strlen(string);
    for (size_t i = 0; i < len; i++)
    {
        if(string[i] == '\n')
        {
            vga_pos_y++;
            vga_pos_x=0;
        }
        else
        {
            print_char_at(string[i],0x0F,vga_pos_x,vga_pos_y);
            vga_pos_x++;
        }
        
    }
    
}