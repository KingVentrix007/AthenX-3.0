#include "stdio.h"
#include "syscall.h"
#include "io.h"
char getchar()
{
    return syscall(SYS_GETC,0,0);
}