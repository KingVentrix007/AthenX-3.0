#ifndef SYS_CALLS_H
#define SYS_CALLS_H

// Define syscall numbers for AthenX-3.0

#define SYS_PRINT           0   // Print to console
#define SYS_ALLOCATE_MEM    1   // Allocate memory
#define SYS_FREE_MEM        2   // Free memory
#define SYS_OPEN_FILE       3   // Open a file
#define SYS_CLOSE_FILE      4   // Close a file
#define SYS_READ_FILE       5   // Read from a file
#define SYS_WRITE_FILE      6   // Write to a file
#define SYS_CREATE_FILE     7   // Create a new file
#define SYS_DELETE_FILE     8   // Delete a file
#define SYS_READ_DIR        9   // Read directory contents
#define SYS_CHANGE_DIR      10  // Change current directory
#define SYS_GET_FILE_SIZE   11  // Get file size
#define SYS_RENAME_FILE     12  // Rename a file
#define SYS_COPY_FILE       13  // Copy a file
#define SYS_MOVE_FILE       14  // Move a file
#define SYS_GET_TIME        15  // Get current system time
#define SYS_PUTS 17
int sys_call_puts(char chr);
#endif /* SYS_CALLS_H */
