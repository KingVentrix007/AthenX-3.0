

#include "syscall.h"
#ifndef FREESTANDING_MODE // BEING USED IN ATHENX
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
#elif defined FREESTANDING_MODE // NOT BEING USED IN ATHENX
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
#endif