#ifndef __TERMINAL__H
#define __TERMINAL__H
#include "stdint.h"

int init_terminal(int x,int y);
int draw_vbe_char(char c);
void draw_char_8x16(int x, int y, char character);
int get_terminal_postion_y(void);
int get_terminal_postion_x(void);
int set_terminal_postion_y(int y);
int set_terminal_postion_x(int y);
uint32_t get_font_bg();
void set_scroll_x_pos(uint32_t x);
size_t get_terminal_hight();
size_t get_terminal_width();
#endif