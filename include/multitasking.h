#ifndef __MULTITASKING__H
#define __MULTITASKING__H

#include "types.h"
typedef struct {
    int process_id;
    uint32 ds;
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by pusha
    uint32 int_no, err_code;                        // interrupt number and error code
    uint32 eip, cs, eflags, useresp, ss;            // pushed by the processor automatically

    // Additional context information
    uint32 cr3; // Page directory base register for virtual memory management
    uint32 priority; // Priority of the process for scheduling
    uint32 state; // Process state (e.g., running, waiting, sleeping)

    // Memory management
    uint32 page_directory[1024]; // Page directory for the process
    uint32 page_table[1024]; // Page table for the process

    // File system related
    // Add fields related to open files, current directory, etc.

    // IPC (Inter-Process Communication)
    // Add fields for message queues, semaphores, etc.

    // Networking
    // Add fields for socket information, IP address, etc.

    // Other process-related information
} ProcessControlBlock;
#endif