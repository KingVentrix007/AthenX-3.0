#include "vesa.h"
#include "stdint.h"
#include "stddef.h"
#include "termianl.h"
size_t terminal_postion_x;
size_t terminal_postion_y;
size_t terminal_font_width;
size_t terminal_font_height;
extern unsigned char arr_8x16_font[];


int init_terminal(int x, int y)
{
    terminal_postion_x = 0;
    terminal_postion_y = 0;
    terminal_font_width = 8;
    terminal_font_height = 16;
}
int get_terminal_postion_x(void)
{
    return terminal_postion_x;
}
int get_terminal_postion_y(void)
{
    return terminal_postion_y;
}
int set_terminal_postion_x(int x)
{
    terminal_postion_x = x;
}
int set_terminal_postion_y(int y)
{
    terminal_postion_y = y;
}
int draw_vbe_char(char c)
{
    if(c != '\n' && c != '\b' && c != '\t')
    {
        if(terminal_postion_x > 1024-terminal_font_width)
        {
            terminal_postion_x=0;
            terminal_postion_y = terminal_postion_y + terminal_font_height;
        }
        update_cursor_manual();
        draw_char_8x16(terminal_postion_x, terminal_postion_y, c);
        terminal_postion_x+=terminal_font_width;
    }
    else if(c == '\n')
    {
        update_cursor_manual();
        if(terminal_postion_y >= 768-(terminal_font_height*2))
        {
            vesa_scroll(terminal_font_height*2);
            terminal_postion_x = 0;
            // printf("768 reached\n");
            // for(;;);
            terminal_postion_y=terminal_postion_y-(terminal_font_height);
        }
        else
        {
            terminal_postion_x = 0;
            terminal_postion_y+=terminal_font_height;
        }
        
        
    }
    else if (c == '\b')
    {
        if(terminal_postion_x > 1024-terminal_font_width)
        {
            terminal_postion_x=0;
            terminal_postion_y = terminal_postion_y + terminal_font_height;
        }
        update_cursor_manual();
        draw_char_8x16(terminal_postion_x, terminal_postion_y, ' ');
        terminal_postion_x-=terminal_font_width;
    }
    else if (c == '\t')
    {
        terminal_postion_x = terminal_postion_x+(terminal_font_width*4);
    }
    if(multi_buffers_enabled == 1)
    {
        

    }
    
    
    
    
    

}
void draw_char_8x16(int x, int y, char character) {
    // Assuming each character is represented by 8 bytes in the font array
    int charWidth = 8;
    int charHeight = 16;

    // Calculate the starting index in the font array for the given character
    int startIndex = (int)character * charHeight;

    // Loop through each row of the character
    for (int row = 0; row < charHeight; row++) {
        // Get the corresponding byte from the font array
        unsigned char fontByte = arr_8x16_font[startIndex + row];

        // Loop through each pixel in the row
        for (int col = 0; col < charWidth; col++) {
            // Check if the current pixel is set in the font byte
            if ((fontByte >> (7 - col)) & 1) {
                if(multi_buffers_enabled == 0)
                {
                    vbe_putpixel(x + col, y + row, vbe_rgb(255, 255, 255)); // Assuming white color for the character

                }
                else
                {
                    // printf_com("%c",character);
                    draw_pixel_buffer_1(x + col, y + row, vbe_rgb(255, 255, 255));
                }
                // Set the pixel at the corresponding position
            }
        }
    }

}