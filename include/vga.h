#ifndef __VGA__H
#define __VGA__H

#include <stdint.h>
void print_char(char character, uint8_t color);
void kprints(const char *string);
void print_char_at(char character, uint8_t color, int x, int y);
#endif