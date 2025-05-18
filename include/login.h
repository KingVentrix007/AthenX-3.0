#ifndef __LOGIN__H
#define __LOGIN__H
#include "stdint.h"
#define THEME 1
#define COLOR_BG 0x000000  // Black background

#if THEME == 1
// 1990s-inspired CRT aesthetic theme
#define COLOR_TEXT 0x00FF00  // Bright green text color (classic monochrome look)
#define COLOR_FIELD_BORDER 0x006400  // Dark green border (subtle contrast for text areas)
#define COLOR_FIELD_SHADOW 0x003200  // Very dark green shadow
#define COLOR_FIELD_HIGHLIGHT 0x00AA00  // Slightly lighter green for highlights
#else
// Optional alternate theme for amber display
#define COLOR_TEXT 0xFFA500  // Bright amber text color
#define COLOR_FIELD_BORDER 0xCC8400  // Dark amber border
#define COLOR_FIELD_SHADOW 0x663400  // Deep amber shadow
#define COLOR_FIELD_HIGHLIGHT 0xFFB732  // Light amber highlight
#endif

extern uint32_t username_start_x;
extern uint32_t username_start_y;

extern uint32_t password_start_x;
extern uint32_t password_start_y;

extern uint32_t command_start_x, command_start_y;
extern uint32_t menu_start_x, menu_start_y;
extern uint32_t output_start_x, output_start_y;
extern uint32_t score_start_x, score_start_y;
char *get_password(char *s, int n);
void draw_ui_layout(int screenX, int screenY);
#endif