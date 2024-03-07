#include "syscall.h"
#include "stdint.h"
#include "stddef.h"
#include "types.h"
#include "sys/types.h"
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

int sys_read(int stream,void *buf,size_t count, ssize_t *bytes_read)
{

}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset)
{

}
int sys_close(int fd)
{

}
void sys_libc_panic()
{

}
int sys_anon_allocate(size_t size, void **pointer)
{

}
int sys_anon_free(void *pointer, size_t size)
{

}