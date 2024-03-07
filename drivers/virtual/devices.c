#include "virtual/devices.h"
#include "string.h"
#include "stdlib.h"
#include "vesa.h"
#include "stdio.h"
#include "io_ports.h"
int dev_tty = 0xEE1;
int dev_ttySO = 0xEE2;
int dev_random = 0xEE3;
int dev_mem = 0xEE4;
int dev_null = 0xEE5;
int dev_fmb = 0xEE6;
VirtualDevice *decode_device(char *path);
void *handle_virtual_device_fopen(char *device, char *mods)
{
    // Decode the device path to get the corresponding VirtualDevice
    VirtualDevice *dev = decode_device(device);
    
    if (dev == NULL) {
        // Handle invalid device path
        return NULL;
    }

    // Check the file mode (mods) and set the corresponding flags in the VirtualDevice
    if (strcmp(mods, "r") == 0 || strcmp(mods, "rb") == 0 || strcmp(mods, "r+") == 0 || strcmp(mods, "rb+") == 0) {
        // Set the read flag
        dev->read_flag = 1;
    } 
    else if (strcmp(mods, "w") == 0 || strcmp(mods, "wb") == 0 || strcmp(mods, "w+") == 0 || strcmp(mods, "wb+") == 0) {
        // Set the write flag
        dev->write_flag = 1;
    } 
    else {
        // Handle unsupported file modes
        // You may want to add more error handling logic here based on your requirements
        return NULL;
    }

    // Return the VirtualDevice pointer with flags set
    return dev;
}


int is_virtual_device_path(char *path)
{
    if(strncmp(path,"/dev/",5) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


VirtualDevice *decode_device(char *path)
{
    VirtualDevice *dev = malloc(sizeof(VirtualDevice));
    if (dev == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    if (strcmp(path, "/dev/tty") == 0)
    {
        // Handle virtual terminal device (/dev/tty)
        dev->device_code = dev_tty;
        dev->supports_read = true;
        dev->supports_write = true;
        dev->read = &read_tty;
        dev->write = &write_tty;
    }
    else if (strcmp(path, "/dev/ttySO") == 0)
    {
        // Handle another virtual terminal device (/dev/ttySO)
        dev->device_code = dev_ttySO;
        dev->supports_read = false;
        dev->supports_write = true;
        dev->read = NULL;
        dev->write = &write_tty;
    }
    else if (strcmp(path, "/dev/random") == 0)
    {
        // Handle random number generator device (/dev/random)
        dev->device_code = dev_random;
        dev->supports_write =  false;
        dev->supports_read = false;
        dev->write = NULL;
        dev->read = NULL;
    }
    else if (strcmp(path, "/dev/mem") == 0)
    {
        // Handle memory device (/dev/mem)
        dev->device_code = dev_mem;
        dev->supports_write = false;
        dev->supports_read = true;
        dev->write = NULL;
        dev->read = &read_mem;
    }
    else if (strcmp(path, "/dev/null") == 0)
    {
        // Handle null device (/dev/null)
        dev->device_code = dev_null;
        dev->supports_write = true;
        dev->supports_read = false;
        dev->read = NULL;
        dev->write = &write_NULL;

    }
    else if (strcmp(path, "/dev/fmb") == 0)
    {
        dev->device_code = dev_fmb;
        dev->supports_read = false;
        dev->supports_write = true;
        dev->write = &write_fmb;
        dev->read = NULL;
    }
    
    else
    {
        // Handle unknown device or invalid path
        free(dev);
        return NULL;
    }

    return dev;
}


//Device read function
int device_read(void *ptr, size_t size, size_t nmemb, void *stream)
{
    VirtualDevice *dev = (VirtualDevice *)stream;
    if(dev->supports_read == true && dev->read_flag == 1)
    {
        if(dev->read == NULL)
        {
            return -1;
        }
        else
        {
            int bytesRead = dev->read(ptr, 0, nmemb);
        }
    }
}
int device_write(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    VirtualDevice *dev = (VirtualDevice *)stream;
    if(dev->supports_write == true && dev->write_flag == 1)
    {
        if(dev->write == NULL)
        {
            return -1;
        }
        else
        {
            return dev->write(ptr,0,nmemb);
        }
    }
}

/*=============================================================================================================*/
                                    /*Device access implantation*/
/*=============================================================================================================*/
//TTY functions
int write_tty(char *buf,int pos,size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        fputc(buf[i],stdout);
    }
    
}
int read_tty(char *buf, int pos, size_t count)
{
    // Allocate memory for the buffer
    char *buffer = malloc(count * sizeof(char));
    if (buffer == NULL)
    {
        // Handle memory allocation failure
        // You might want to return an error code or handle it according to your requirements
        return -1;
    }

    // Read characters from standard input into the buffer
    for (size_t i = 0; i < count; i++)
    {
        buffer[i] = fgetc(stdin);
    }

    // Copy the contents of the buffer into the provided 'buf' parameter
    memcpy(buf, buffer, count);

    // Free the allocated memory for the buffer
    free(buffer);
    return 1;
}
//FMB functions
int write_fmb(char *buf,int pos,size_t count)
{
    draw_pixel_buffer_1_tty(pos,count,buf);
}
int read_fmb(char *buf,int pos,size_t count)
{
    return -1;
}
//TTYSO(serial) functions
int write_ttySO(char *buf,int pos,size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        write_to_com1(buf[i]);
    }
    
}
int read_ttySO(char *buf,int pos,size_t count)
{
    return -1;
}
//MEM functions
int read_mem(char *buf,int pos,size_t count)
{
    
}
//NULL functions
int write_NULL(char *buf,int pos,size_t count)
{
    
}