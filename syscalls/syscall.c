#include "syscall.h"
#include "printf.h"
// #include "mem.h"
#include "fat_filelib.h"
int system_call_handler_c(int syscall_number, int param1, int param2)
{
    // printf("system_call_handler_c: %d\n", syscall_number);
    if(syscall_number == SYS_LOG)
    {
        char *message = (char *)param1;
        printf("%s\n", message);
    }
    else if(syscall_number == SYS_PANIC)
    {
        printf("PANIC\n");
        for(;;);
    }
    else if(syscall_number == SYS_TCB_SET)
    {
        // Handle TCB_SET syscall
    }
    else if(syscall_number == SYS_FUTEX_TID)
    {
        // Handle FUTEX_TID syscall
    }
    else if(syscall_number == SYS_FUTEX_WAIT)
    {
        // Handle FUTEX_WAIT syscall
    }
    else if(syscall_number == SYS_FUTEX_WAKE)
    {
        // Handle FUTEX_WAKE syscall
    }
    else if(syscall_number == SYS_ANON_ALLOCATE)
    {
        void *ptr = kmalloc(param1);
        return ptr;
    }
    else if(syscall_number == SYS_ANON_FREE)
    {
        kfree(param1);
    }
    else if(syscall_number == SYS_OPEN)
    {
        FILE *file = fl_fopen(param1, param2);
    }
    else if(syscall_number == SYS_READ)
    {
        // Handle READ syscall
    }
    else if(syscall_number == SYS_SEEK)
    {
        // fl_fread();
        // Handle SEEK syscall
    }
    else if(syscall_number == SYS_CLOSE)
    {
        // Handle CLOSE syscall
    }
    else if(syscall_number == SYS_STAT)
    {
        // Handle STAT syscall
    }
    else if(syscall_number == SYS_VM_MAP)
    {
        // Handle VM_MAP syscall
    }
    else if(syscall_number == SYS_VM_UNMAP)
    {
        // Handle VM_UNMAP syscall
    }
    else if(syscall_number == SYS_VM_PROTECT)
    {
        // Handle VM_PROTECT syscall
    }
}
//Triggers system call
int syscall(int syscall_number, int param1, int param2) {
    int result;
    asm volatile ( 
        "int $42"
        : "=a" (result)
        : "a" (syscall_number), "b" (param1), "c" (param2)
    );
    // if(syscall_number == SYS_GET_X)
    // {
    // }
    return result;
}