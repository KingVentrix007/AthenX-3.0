#include "printf.h"
#include "stdarg.h"
#include "vesa.h"
#include "stdint.h"
#include "stddef.h"
#include "debug_term.h"
#include "io_ports.h"
size_t debug_terminal_postion_x;
size_t debug_terminal_postion_y;
size_t debug_terminal_font_width;
size_t debug_terminal_font_height;
extern unsigned char arr_8x16_font[];
int printf_debug(const char *fmt, ...)
{
    char buffer[256]; // Adjust the buffer size as needed

    // Initialize the variable argument list
    va_list args;
    va_start(args, fmt);

    // Format the string using vsnprintf
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    if(multi_buffers_enabled == 1)
    {
         for (size_t i = 0; i < strlen(buffer); i++)
        {
            write_to_com1(buffer[i]);
            draw__debug_vbe_char(buffer[i]);
        }
    }
    char *msg = "DEBUG: ";
    write_to_com1_string(msg);
    write_to_com1_string(buffer);
    
    
    // Cleanup the va
}



int init_debug_terminal(int x, int y)
{
    debug_terminal_postion_x = 0;
    debug_terminal_postion_y = 0;
    debug_terminal_font_width = 8;
    debug_terminal_font_height = 16;
}
int get_debug_terminal_postion_x(void)
{
    return debug_terminal_postion_x;
}
int get_debug_terminal_postion_y(void)
{
    return debug_terminal_postion_y;
}
int set_debug_terminal_postion_x(int x)
{
    debug_terminal_postion_x = x;
}
int set_debug_terminal_postion_y(int y)
{
    debug_terminal_postion_y = y;
}
int draw__debug_vbe_char(char c)
{
    if(c != '\n' && c != '\b' && c != '\t')
    {
        if(debug_terminal_postion_x > 1024-debug_terminal_font_width)
        {
            debug_terminal_postion_x=0;
            debug_terminal_postion_y = debug_terminal_postion_y + debug_terminal_font_height;
        }
        update_cursor_manual();
        draw__debug_char_8x16(debug_terminal_postion_x, debug_terminal_postion_y, c);
        debug_terminal_postion_x+=debug_terminal_font_width;
    }
    else if(c == '\n')
    {
        update_cursor_manual();
        if(debug_terminal_postion_y >= 768-(debug_terminal_font_height*2))
        {
            debug_scroll(debug_terminal_font_height*2);
            debug_terminal_postion_x = 0;
            // printf("768 reached\n");
            // for(;;);
            debug_terminal_postion_y=debug_terminal_postion_y-(debug_terminal_font_height);
        }
        else
        {
            debug_terminal_postion_x = 0;
            debug_terminal_postion_y+=debug_terminal_font_height;
        }
        
        
    }
    else if (c == '\b')
    {
        if(debug_terminal_postion_x > 1024-debug_terminal_font_width)
        {
            debug_terminal_postion_x=0;
            debug_terminal_postion_y = debug_terminal_postion_y + debug_terminal_font_height;
        }
        update_cursor_manual();
        draw__debug_char_8x16(debug_terminal_postion_x, debug_terminal_postion_y, ' ');
        debug_terminal_postion_x-=debug_terminal_font_width;
    }
    else if (c == '\t')
    {
        debug_terminal_postion_x = debug_terminal_postion_x+(debug_terminal_font_width*4);
    }
    if(multi_buffers_enabled == 1)
    {
        

    }
    
    
    
    
    

}
void draw__debug_char_8x16(int x, int y, char character) {
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
                    // vbe_putpixel(x + col, y + row, vbe_rgb(255, 255, 255)); // Assuming white color for the character

                }
                else
                {
                    // dbgprintf("%c",character);
                    draw_pixel_buffer_2(x + col, y + row, vbe_rgb(255, 255, 255));
                }
                // Set the pixel at the corresponding position
            }
        }
    }
}

int kernel_panic(char *caller, char *reason)
{
    cls();
    printf("Kernel panicked, There is nothing you can do\n");
    printf("%s -> %s\n", caller, reason);
    for(;;);
}