#include "../include/syscall.h"
#include "printf.h"
#include "keyboard.h"
#include "stdio.h"
#include "stdlib.h"
int system_call_handler_c(int syscall_number, int param1, int param2)
{
    // if(syscall_number == )
    if(syscall_number == -1)
    {

    }

    if(syscall_number == SYS_PUTS)
    {
        printf("%c", param1);
    }
    else if(syscall_number == 21)
    {
        printf("%s", param1);
        printf("\n\n");
        return fputs((char *)param1,(void *)param2);
    }

    
}