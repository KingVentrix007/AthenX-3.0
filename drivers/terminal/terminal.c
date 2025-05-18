#include "vesa.h"
#include "stdint.h"
#include "stddef.h"
#include "termianl.h"
size_t terminal_postion_x;
size_t terminal_postion_y;
size_t terminal_font_width;
size_t terminal_font_height;
size_t terminal_width;
size_t terminal_height;
size_t scroll_count = 0;
uint32_t font_fg_color;
uint32_t font_bg_color;
uint32_t scroll_x_pos = 0;

extern unsigned char arr_8x16_font[];
void set_font_bg_color_(int color);
void set_font_fg_color_(int color);
uint32_t get_font_fg()
{
    return font_fg_color;
}
uint32_t get_font_bg()
{
    return font_bg_color;
}
void set_scroll_x_pos(uint32_t x)
{
    scroll_x_pos = x;
}
void set_font_fg_color_(int color)
{
    font_fg_color = color;
}
void set_font_bg_color_(int color)
{
    font_bg_color = color;
}

void set_font_fg_color(int color_index) {
    switch (color_index) {
        case 0: // Black
            font_fg_color = vbe_rgb(0, 0, 0);
            break;
        case 1: // Red
            font_fg_color = vbe_rgb(255, 0, 0);
            break;
        case 2: // Green
            font_fg_color = vbe_rgb(0, 255, 0);
            break;
        case 3: // Yellow
            font_fg_color = vbe_rgb(255, 255, 0);
            break;
        case 4: // Blue
            font_fg_color = vbe_rgb(0, 0, 255);
            break;
        case 5: // Magenta
            font_fg_color = vbe_rgb(255, 0, 255);
            break;
        case 6: // Cyan
            font_fg_color = vbe_rgb(0, 255, 255);
            break;
        case 7: // White
            font_fg_color = vbe_rgb(255, 255, 255);
            break;
        default:
            // Invalid color index
            printf("Invalid color index.(%d)\n",color_index);
            break;
    }
}

// Function to set font background color based on index
void set_font_bg_color(int color_index) {
    switch (color_index) {
        case 0: // Black
            font_bg_color = vbe_rgb(0, 0, 0);
            break;
        case 1: // Red
            font_bg_color = vbe_rgb(255, 0, 0);
            break;
        case 2: // Green
            font_bg_color = vbe_rgb(0, 255, 0);
            break;
        case 3: // Yellow
            font_bg_color = vbe_rgb(255, 255, 0);
            break;
        case 4: // Blue
            font_bg_color = vbe_rgb(0, 0, 255);
            break;
        case 5: // Magenta
            font_bg_color = vbe_rgb(255, 0, 255);
            break;
        case 6: // Cyan
            font_bg_color = vbe_rgb(0, 255, 255);
            break;
        case 7: // White
            font_bg_color = vbe_rgb(255, 255, 255);
            break;
        default:
            // Invalid color index
            printf("Invalid color index.\n");
            break;
    }
}
int init_terminal(int x, int y)
{
    font_fg_color = vbe_rgb(255, 255, 255);
    font_bg_color = vbe_rgb(0, 0, 0);
    terminal_postion_x = 0;
    terminal_postion_y = 0;
    terminal_font_width = 8;
    terminal_font_height = 16;
    terminal_width = x;
    terminal_height = y;
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
        if(terminal_postion_x > terminal_width-terminal_font_width)
        {
            terminal_postion_x=scroll_x_pos;
            terminal_postion_y = terminal_postion_y + terminal_font_height;
        }
        update_cursor_manual();
        draw_char_8x16(terminal_postion_x, terminal_postion_y, c);
        terminal_postion_x+=terminal_font_width;
    }
    else if(c == '\n')
    {
        update_cursor_manual();
        if(terminal_postion_y >= terminal_height-(terminal_font_height*2))
        {
            vesa_scroll(terminal_font_height*2);
            terminal_postion_x = scroll_x_pos;
            // printf("768 reached\n");
            // for(;;);
            terminal_postion_y=terminal_postion_y-(terminal_font_height);
        }
        else
        {
            terminal_postion_x = scroll_x_pos;
            terminal_postion_y+=terminal_font_height;
        }
        
        
    }
    else if (c == '\b')
    {
        if(terminal_postion_x > terminal_width-terminal_font_width)
        {
            terminal_postion_x=scroll_x_pos;
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
                // Check if multi-buffering is enabled
                if (multi_buffers_enabled == 0) {
                    // Single buffer mode
                    vbe_putpixel(x + col, y + row, font_fg_color);
                } else {
                    // Multi-buffer mode
                    draw_pixel_buffer_1(x + col, y + row, font_fg_color);
                }
            } else {
                // Draw the background color if the pixel is not set
                if (multi_buffers_enabled == 0) {
                    // Single buffer mode
                    vbe_putpixel(x + col, y + row, font_bg_color);
                } else {
                    // Multi-buffer mode
                    draw_pixel_buffer_1(x + col, y + row, font_bg_color);
                }
            }
        }
    }
}
int scroll_up()
{
    vesa_scroll((terminal_font_height*2)*-1);
    scroll_count++;
    // terminal_postion_x = 0;
    //  terminal_postion_y=terminal_postion_y+(terminal_font_height);
}
int scroll_down()
{
    vesa_scroll((terminal_font_height*2));
    // terminal_postion_x = 0;
}
int reset_poss_from_scroll()
{
    if(scroll_count > 0)
    {   
     vesa_scroll((terminal_font_height*2)*scroll_count);
    scroll_count = 0;

    }

}
void reset_terminal_settings()
{
    font_bg_color = vbe_rgb(0,0,0);
    font_fg_color = vbe_rgb(255,255,255);
}