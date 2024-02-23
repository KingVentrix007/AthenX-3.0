#include "fat_filelib.h"
#include "kheap.h"
#include "logging.h"
#include "stdbool.h"
#include "file.h"
#include "stdio.h"
#include "io_ports.h"
char *_cwd;
size_t _cwd_len;
char _fixed_cwd[FATFS_MAX_LONG_FILENAME];
bool use_cwd = true;
int stdin = STDIN_VALUE;
int stdout = STDOUT_VALUE;
int stderr = STDERR_VALUE;
drive_info drives[24];
int init_fs()
{
    _cwd = kmalloc(FATFS_MAX_LONG_FILENAME);
    if(_cwd == NULL)
    {
        logging(3,__LINE__,__func__,__FILE__,"FS: Failed to allocate memory for _cwd. Defaulting to fixed size buffer\nUsing fixed size buffer");
        use_cwd = false;
        return -1;
    }
    else
    {
        _cwd_len = 0;
        return 1;
    }
    
}


int chdir(const char *path)
{
    if(use_cwd == true)
    {
        char *tmp = kmalloc(_cwd_len+strlen(path)+3);
        // strcat(tmp,"");
        strcpy(tmp,_cwd);
        strcat(tmp,"/");
        strcat(tmp,path);
        if(fl_is_dir(tmp) == 0)
        {
            strcpy(_cwd,tmp);
            _cwd_len = _cwd_len+1+strlen(path)+2;
            kfree(tmp);
            return 0;
        }
        kfree(tmp);
        return -1;
    }
    else
    {
        char tmp[FATFS_MAX_LONG_FILENAME];
        strcpy(tmp,_fixed_cwd);
        strcat(tmp,"/");
        strcat(tmp,path);
        
    }

}

char *getcwd()
{
    if(use_cwd)
    {
        return _cwd;
    }
    else
    {
        return _fixed_cwd;
    }
}

void *fopen(const char *path,const char *modifiers)
{
    if(1 == 0)//If fat32 or ext#, ATM only fat works
    {
        return fl_fopen(path,modifiers);
    }
    else if (2 == 1)
    {
        //EXT not supported yet
    }
    
}
/**
 * Function Name: fread
 * Description: Reads data from the given file stream.
 *
 * Parameters:
 *   ptr (void*) - Pointer to the memory area where data will be read into.
 *   size (size_t) - Size of each element to be read.
 *   nmemb (size_t) - Number of elements to read.
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream to read from.
 *
 * Return:
 *   size_t - Number of elements successfully read.
 */
size_t fread(void *ptr, size_t size, size_t nmemb, void *stream) {

    if((int)stream == stdin)
    {   
        char *buf = (char *)ptr;
        for (size_t i = 0; i < nmemb; i++)
        {
            char chr;
            while(chr = getchar() <= 0)
            {

            }
            buf[i] = chr;
        }
        ptr = buf;
        
    }
    else if(1 == 0)
    {
        return fl_fread(ptr,size,nmemb,stream);
    }
    else if (0 == 2)
    {
        // EXT not supported
    }
    
    return 0;
}

/**
 * Function Name: fwrite
 * Description: Writes data to the given file stream.
 *
 * Parameters:
 *   ptr (const void*) - Pointer to the array containing the data to be written.
 *   size (size_t) - Size of each element to be written.
 *   nmemb (size_t) - Number of elements to write.
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream to write to.
 *
 * Return:
 *   size_t - Number of elements successfully written.
 */
size_t fwrite(const void *ptr, size_t size, size_t nmemb, void *stream) {
    if((int)stream == stdout)
    {
        char *buf = (char *)ptr;
        for (size_t i = 0; i < nmemb; i++)
        {
            printf("%c", buf[i]);
        }
        buf = NULL;
        return nmemb;
        

    }
    else if ((int)stream == stderr)
    {
        char *buf = (char *)ptr;
        for (size_t i = 0; i < nmemb; i++)
        {
            printf_com("%c",buf[i]);
        }
        return nmemb;

        
        
    }
    else
    {
        if(1 == 0)
        {
            return fl_fwrite(ptr,size,nmemb,stream);
        }
        else
        {
            //EXT not supported
        }
    }
    return 0;
}

/**
 * Function Name: fputc
 * Description: Writes a character to the specified stream.
 *
 * Parameters:
 *   c (int) - The character to be written.
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream to write to.
 *
 * Return:
 *   int - On success, returns the written character as an unsigned char cast to an int.
 *         On failure, returns EOF.
 */
int fputc(int c, void *stream) {
    if((int)stream == stdout)
    {
        printf("%c", c);
        return c;
    }
    else if ((int)stream == stderr)
    {
        write_to_com1(c);
        return c;
    }
    else
    {
        return fl_fputc(c,stream);
    }
    
    // Empty implementation
    return EOF;
}

/**
 * Function Name: fputs
 * Description: Writes a string to the specified stream.
 *
 * Parameters:
 *   str (const char*) - Pointer to the null-terminated string to be written.
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream to write to.
 *
 * Return:
 *   int - On success, returns a non-negative value.
 *         On failure, returns EOF.
 */
int fputs(const char *str, void *stream) {
    if((int)stream == stdout)
    {
        printf("%s", str);
        return 1;
    }
    else if ((int)stream == stderr)
    {
        printf_com("%s", str);
        return 1;
    }
    else if ((int)stream != stdin)
    {
        return fl_fputs(str,stream);
    }
    
    
    // Empty implementation
    return EOF;
}

/**
 * Function Name: fclose
 * Description: Closes the given file stream.
 *
 * Parameters:
 *   stream (FILE*) - Pointer to a FILE object to be closed.
 *
 * Return:
 *   int - On success, returns 0.
 *         On failure, returns EOF.
 */
int fclose(void *stream) {
     fl_fclose(stream);
     return 1;
}

/**
 * Function Name: fseek
 * Description: Sets the file position indicator for the given file stream.
 *
 * Parameters:
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream.
 *   offset (long) - Number of bytes to offset from the origin.
 *   whence (int) - Position from where offset is added (SEEK_SET, SEEK_CUR, SEEK_END).
 *
 * Return:
 *   int - On success, returns 0.
 *         On failure, returns non-zero value.
 */
int fseek(void *stream, long offset, int whence) {
    if((int)stream == stdin)
    {
        long new_position;
        switch(whence) {
        case SEEK_SET:
            new_position = offset;
            break;
        case SEEK_CUR:
            new_position = offset + ftell(stream); // Assuming ftell() gives the current position
            break;
        case SEEK_END:
            logging(3,__LINE__,__func__,__FILE__,"ATTEMPT TO READ OUT OF BUFFER");
            return -1;
            // new_position = buffer_size + offset;
            // break;
        default:
            return -1; // Invalid origin
        
        set_io_pos(new_position);
    }
    }
    return fl_fseek(stream,offset,whence);
    // return -1;
}

/**
 * Function Name: fgetpos
 * Description: Gets the current file position.
 *
 * Parameters:
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream.
 *   pos (fpos_t*) - Pointer to store the current file position.
 *
 * Return:
 *   int - On success, returns 0.
 *         On failure, returns a non-zero value.
 */
int fgetpos(void *stream, fpos_t *pos) {
    return fl_fgetpos(stream, pos);
    // Empty implementation
    // return -1;
}

/**
 * Function Name: ftell
 * Description: Gets the current file position.
 *
 * Parameters:
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream.
 *
 * Return:
 *   long - On success, returns the current offset in the stream.
 *          On failure, returns -1L.
 */
long ftell(void *stream) {
    if((int)stream == stdin)
    {
        return get_io_pos();
    }
    return fl_ftell(stream);
    // Empty implementation
    // return -1L;
}

/**
 * Function Name: feof
 * Description: Checks if the end-of-file indicator associated with the stream is set.
 *
 * Parameters:
 *   stream (FILE*) - Pointer to a FILE object that specifies the stream.
 *
 * Return:
 *   int - Returns non-zero if end-of-file indicator is set, otherwise returns 0.
 */
int feof(void *stream) {
    return fl_feof(stream);
    // Empty implementation
    return 0;
}
char *fgets(char *s, int n, void *f) {
    if ((int)f == stdin) {
        char ch;
        int count = 0;
        while (((ch = getch_()) > 0 && ch != '\n' && ch != EOF) && count < n - 1) {
            if (ch == '\b') {
                if (count > 0) { // Ensure not underflowing the buffer
                    s--;
                    count--;
                }
            } else {
                *s = ch;
                s++;
                count++;
            }
        }
        *s = '\0'; // Null-terminate the string
    } else {
        return fl_fgets(s, n, f); // Use the standard library fgets for other file streams
    }
    return s; // Return the pointer to the string
}
/**
 * Function Name: fgetc
 * Description: Implementation of fgetc to read a single character from standard input.
 *              Utilizes similar logic to custom fgets function.
 *
 * Parameters:
 *   f (void *) - Pointer to the file stream. In this case, it should be stdin.
 *
 * Return:
 *   int - The next character from the input stream, or EOF if the end of the file or an error occurs.
 */
int fgetc(void *f) {
    if ((int)f == stdin) {
        char ch;
        ch = getch_(); // Assuming getch_() is a function to get a single character from input
        return (ch == EOF) ? EOF : (unsigned char)ch; // Return EOF or the character read
    } else {
        return fl_fgetc(f); // Use the standard library fgetc for other file streams
    }
}
/**
 * Function Name: mkdir
 * Description: Creates a new directory with the specified name.
 *
 * Parameters:
 *   path (const char*) - Pointer to a null-terminated string containing the directory name.
 *
 * Return:
 *   int - On success, returns 0.
 *         On failure, returns -1.
 */
int mkdir(const char *path) {

    return fl_createdirectory(path);
    // return -1;
}

/**
 * Function Name: rmdir
 * Description: Removes the specified directory.
 *
 * Parameters:
 *   path (const char*) - Pointer to a null-terminated string containing the directory name.
 *
 * Return:
 *   int - On success, returns 0.
 *         On failure, returns -1.
 */
int rmdir(const char *path) {

    // Empty implementation
    return -1;
}
/**
 * Function Name: fprintf
 * Description: Writes formatted data to a stream.
 *
 * Parameters:
 *   fp (void*) - Pointer to the FILE object where the data will be written.
 *   format (const char*) - Format string specifying how subsequent arguments are converted for output.
 *   ... - Additional arguments that will be formatted according to the format string.
 *
 * Return:
 *   Returns the number of characters written on success; a negative value is returned if an error occurs.
 */
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
    if((int)fp == stdout)
    {
        printf("%s",buffer);
    }
    else if ((int)fp == stderr)
    {
        printf_com("%s",buffer);
    }
    else
    {
        int write_ret = fwrite(buffer,sizeof(char),strlen(buffer),fp);
    }
    
    // printf("write_ret = %d\n",write_ret);
    memset(buffer,0,1024*3);
    // kfree(buffer);
}

int ungetc(int c,void *stream)
{
    if((int)stream == stdin)
    {
        push_io(c);
        return c; 
    }
    else
    {
        return fputc(c,stream);
    }
}

int fflush(int stream)
{
    return 0;
}
