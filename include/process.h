#ifndef __PROCESS__H
#define __PROCESS__H

#include "types.h"
#include "scheduler.h"
#include "stdint-gcc.h"
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
typedef struct {
    ProcessControlBlock* task;       // Pointer to the ProcessControlBlock
    void (*taskFunction)(void);     // Pointer to the task function
} TaskEntry;

TaskEntry taskList[40];       // Updated taskList definition

// typedef struct {
//     void* kernel_stack_top;  // Pointer to the top of the task's kernel stack
//     uint32_t virtual_address_space;  // Virtual address space (e.g., CR3 on x86 systems)
//     struct tcb* next_task;  // Pointer to the next task in a linked list (for scheduling)
//     uint32_t state;  // State of the task (e.g., running, waiting, sleeping)
//     // Other optional fields:
//     // int scheduling_policy;
//     // int scheduling_priority;
//     // int process_id;
//     // char task_name[MAX_TASK_NAME_LENGTH];
//     // uint64_t cpu_time_consumed;
//     // Add more fields as needed
// } TCB;
typedef struct thread_control_block {
    void* kernel_stack_top;  // Pointer to the top of the kernel stack
    uint32_t virtual_address_space;  // Virtual address space (e.g., CR3 on x86 systems)
    struct thread_control_block* next_task;  // Pointer to the next task in a linked list
    uint32_t state;  // State of the task (e.g., running, waiting, sleeping)
    // Add more fields as needed
    uint32_t ESP;
    uint32_t CR3;
    uint32_t ESP0;
} TCB;

#endif