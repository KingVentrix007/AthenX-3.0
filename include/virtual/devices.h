#ifndef __VIRTUAL_DEVICES__H
#define __VIRTUAL_DEVICES__H
#include "stddef.h"
#include "stdbool.h"
#define VALID_VIRTUAL_DEVICE 0x45334
typedef struct {
    int valid;
    int device_code;
    // Read / write flags
    bool read_flag;
    bool write_flag;
    // Supports read/write
    bool supports_read;
    bool supports_write;
    // Function pointers for read and write operations
    int (*read)(char *buf, int pos, size_t count);
    int (*write)(char *buf, int pos, size_t count);
} VirtualDevice;
void *handle_virtual_device_fopen(char *device, char *mods);
/**
 * Function Name: write_tty
 * Description: Writes data to the standard output (TTY).
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer containing data to be written.
 *   pos (int) - Not used in this function.
 *   count (size_t) - Number of bytes to write from the buffer.
 *
 * Return:
 *   void
 */
int write_tty(char *buf, int pos, size_t count);

/**
 * Function Name: read_tty
 * Description: Reads data from the standard input (TTY).
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer where read data will be stored.
 *   pos (int) - Not used in this function.
 *   count (size_t) - Number of bytes to read into the buffer.
 *
 * Return:
 *   int - Returns 1 on success, -1 on failure.
 */
int read_tty(char *buf, int pos, size_t count);

/**
 * Function Name: write_fmb
 * Description: Writes data to a framebuffer.
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer containing data to be written.
 *   pos (int) - Position in the framebuffer (not used in this implementation).
 *   count (size_t) - Number of bytes to write to the framebuffer.
 *
 * Return:
 *   void
 */
int write_fmb(char *buf, int pos, size_t count);

/**
 * Function Name: read_fmb
 * Description: Reads data from a framebuffer (not implemented).
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer where read data would be stored.
 *   pos (int) - Position in the framebuffer (not used in this implementation).
 *   count (size_t) - Number of bytes to read from the framebuffer.
 *
 * Return:
 *   int - Always returns -1, as the function is not implemented.
 */
int read_fmb(char *buf, int pos, size_t count);

/**
 * Function Name: write_ttySO
 * Description: Writes data to a serial output (TTY over serial).
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer containing data to be written.
 *   pos (int) - Not used in this function.
 *   count (size_t) - Number of bytes to write to the serial output.
 *
 * Return:
 *   void
 */
int write_ttySO(char *buf, int pos, size_t count);

/**
 * Function Name: read_ttySO
 * Description: Reads data from a serial input (not implemented).
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer where read data would be stored.
 *   pos (int) - Not used in this function.
 *   count (size_t) - Number of bytes to read from the serial input.
 *
 * Return:
 *   int - Always returns -1, as the function is not implemented.
 */
int read_ttySO(char *buf, int pos, size_t count);

/**
 * Function Name: read_mem
 * Description: Reads data from memory (not implemented).
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer where read data would be stored.
 *   pos (int) - Not used in this function.
 *   count (size_t) - Number of bytes to read from memory.
 *
 * Return:
 *   int - Always returns -1, as the function is not implemented.
 */
int read_mem(char *buf, int pos, size_t count);

/**
 * Function Name: write_NULL
 * Description: Writes data to NULL (does nothing).
 *
 * Parameters:
 *   buf (char *) - Pointer to the buffer containing data to be written.
 *   pos (int) - Not used in this function.
 *   count (size_t) - Number of bytes to write (ignored).
 *
 * Return:
 *   int - Always returns 0, indicating success.
 */
int write_NULL(char *buf, int pos, size_t count);

int is_virtual_device_path(char *path);
int is_virtual_device(void *device);
int device_read(void *ptr, size_t size, size_t nmemb, void *stream);
int device_write(const void *ptr, size_t size, size_t nmemb, void *stream);
#endif