#ifndef __TERMINAL__H
#define __TERMINAL__H


int init_terminal(int x,int y);
int draw_vbe_char(char c);
void draw_char_8x16(int x, int y, char character);
int get_terminal_postion_y(void);
int get_terminal_postion_x(void);
#endif