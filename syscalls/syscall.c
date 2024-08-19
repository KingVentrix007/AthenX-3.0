#include "syscall.h"
#include "stdint.h"
#include "stddef.h"
#include "types.h"
#include "sys/types.h"
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