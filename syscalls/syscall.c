#include "syscall.h"
#include "printf.h"
int system_call_handler_c(int syscall_number, void *param)
{
    printf("system_call_handler_c: %d\n", syscall_number);
}
//Triggers system call
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