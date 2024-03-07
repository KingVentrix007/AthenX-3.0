#ifndef DEBUG_FUNCTIONS_H
#define DEBUG_FUNCTIONS_H

#include <stdarg.h>

/**
 * Function Name: printf_debug
 * Description: Print formatted debug output to the debug terminal.
 *
 * Parameters:
 *   fmt (const char*) - The format string for the output.
 *   ... - Variable arguments corresponding to the format string.
 *
 * Return:
 *   void
 */
int printf_debug(const char *fmt, ...);

/**
 * Function Name: init_debug_terminal
 * Description: Initialize the debug terminal with the specified position.
 *
 * Parameters:
 *   x (int) - The x-coordinate for the initial position.
 *   y (int) - The y-coordinate for the initial position.
 *
 * Return:
 *   void
 */
int init_debug_terminal(int x, int y);

/**
 * Function Name: get_debug_terminal_postion_x
 * Description: Get the current x-coordinate of the debug terminal cursor.
 *
 * Parameters:
 *   None
 *
 * Return:
 *   int - The current x-coordinate of the cursor.
 */
int get_debug_terminal_postion_x(void);

/**
 * Function Name: get_debug_terminal_postion_y
 * Description: Get the current y-coordinate of the debug terminal cursor.
 *
 * Parameters:
 *   None
 *
 * Return:
 *   int - The current y-coordinate of the cursor.
 */
int get_debug_terminal_postion_y(void);

/**
 * Function Name: set_debug_terminal_postion_x
 * Description: Set the x-coordinate of the debug terminal cursor.
 *
 * Parameters:
 *   x (int) - The new x-coordinate for the cursor.
 *
 * Return:
 *   void
 */
int set_debug_terminal_postion_x(int x);

/**
 * Function Name: set_debug_terminal_postion_y
 * Description: Set the y-coordinate of the debug terminal cursor.
 *
 * Parameters:
 *   y (int) - The new y-coordinate for the cursor.
 *
 * Return:
 *   void
 */
int set_debug_terminal_postion_y(int y);

/**
 * Function Name: draw__debug_vbe_char
 * Description: Draw a character on the debug terminal using VBE.
 *
 * Parameters:
 *   c (char) - The character to draw.
 *
 * Return:
 *   void
 */
int draw__debug_vbe_char(char c);
int kernel_panic(char *caller, char *reason);
#endif /* DEBUG_FUNCTIONS_H */
