#include "keyboard.h"
#include "kheap.h"
#include "stream.h"
#include "logging.h"
#include "stdio.h"
char *io_stream;
int io_stream_pos = 0;
long io_stream_size = 0;
int init_io_system()
{
    io_stream = kmalloc(IO_STREAM_SIZE);
    io_stream_size = IO_STREAM_SIZE;
    if(io_stream == NULL)
    {
        logging(3,__LINE__,__func__,__FILE__,"%s\n","failed to allocate memory for io_stream");
        return -1;
    }

}

int getchar(void)
{
    if(io_stream_pos <= 0)
    {
        return EOF;
    }
    else if(io_stream != NULL)
    {
        return io_stream[--io_stream_pos];
        
    }
    else
    {
        return EOF;
    }
}
long get_io_pos()
{
    return io_stream_pos;
}
int set_io_pos(int pos)
{
    if(pos <= io_stream_size && pos >= 0)
    {
        io_stream_pos = pos;
        return io_stream_pos;
    }
    else
    {
        return EOF;
    }   
    
}
int push_io(int c)
{
    if(io_stream != NULL || io_stream_pos >= IO_STREAM_SIZE)
    {
        io_stream[io_stream_pos++] = c;
        return io_stream[io_stream_pos-1];
    }
    else
    {
        return EOF;
    }
    
}