

#include "syscall.h"
int syscall(int syscall_number, void *param1, int param2) {
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

int sys_call_puts(char chr)
{
    
    syscall(SYS_PRINT,chr,chr);
}