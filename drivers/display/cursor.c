#include "vesa.h"
#include "stddef.h"
#include "cursor.h"
#include "termianl.h"
#include "io_ports.h"
#define DELAY 3999000
int cursor_visible = 1; // 0: visible 1: invisible
size_t cursor_counter = 0;
void undraw_cursor(int sx, int sy);
void draw_cursor(int sx, int sy);
void draw_cursor(int sx, int sy)
{
    // write_to_com1_string("Draw\n");
    // printf("Drawing cursor\n");
    for (size_t x = 0; x < 8; x++)
    {
        for (size_t y = 0; y < 16; y++)
        {
            if(multi_buffers_enabled == 0)
            {
            vbe_putpixel(sx+x,sy+y,VBE_RGB(32,194,14));

            }
            else
            {
                draw_pixel_buffer_1(sx+x,sy+y,VBE_RGB(32,194,14));
            }
        }
        
    }
   
    
}
void undraw_cursor(int sx, int sy)
{
    // write_to_com1_string("Undraw\n");
     for (size_t x = 0; x < 8; x++)
    {
        for (size_t y = 0; y < 16; y++)
        {
            if(multi_buffers_enabled == 0)
            {
             vbe_putpixel(sx+x,sy+y,VBE_RGB(0,0,0));

            }
            else
            {
                draw_pixel_buffer_1(sx+x,sy+y,VBE_RGB(0,0,0));
            }
        }
        
    }
    if(multi_buffers_enabled == 1)
    {
    // update_pixel_display();

    }
}

void update_cursor(void)
{
    
    if(cursor_visible == 1)
    {
        //
        cursor_counter = 0;
        cursor_visible = 0;
        // cursor_counter = 0
        // printf("%d\n",get_terminal_postion_x());
        int x = get_terminal_postion_x();
        int y = get_terminal_postion_y();
        draw_cursor(x,y);
    }
    else if (cursor_visible == 0)
    {
        cursor_counter = 0;
        cursor_visible = 1;
        int x = get_terminal_postion_x();
        int y = get_terminal_postion_y();
        undraw_cursor(x,y);
    }
    else
    {
        cursor_counter++;
    }
    
}
void update_cursor_manual(void)
{
        // cursor_visible = 1;
        undraw_cursor(get_terminal_postion_x(),get_terminal_postion_y());
        // cursor_counter = 0;
    // if(cursor_visible == 1)
    // {
    //     //
        
    //     cursor_visible = 0;
    //     // cursor_counter = 0
    //     draw_cursor(get_terminal_postion_x()+8,get_terminal_postion_y());
    // }
    
    
        
        
    
}