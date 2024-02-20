#include "fat_filelib.h"
#include "printf.h"
#include "stdarg.h"
#include "kheap.h"
int fprintf(void *fp,const char *format,...)
{
    char buffer[1024*3];
    va_list va;
    va_start(va, format);
    if(buffer == NULL)
    {
        printf("Buffer allocation failed\n");
        return -1;
        // buffer = backup_buffer;

    }
    const int ret = vsnprintf_(buffer, 1024*3, format, va);
    va_end(va);
    // printf("buffer = %s\nlen = %d\n", buffer,strlen(buffer));

    int write_ret = fl_fwrite(buffer,sizeof(char),strlen(buffer),fp);
    // printf("write_ret = %d\n",write_ret);
    memset(buffer,0,1024*3);
    // kfree(buffer);
}