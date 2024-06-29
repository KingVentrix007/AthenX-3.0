#include "storage.h"
#include "fat_access.h"
#include "scanf.h"
#include "fat_filelib.h"
#include "device.h"
int init_file_system(int drive)
{
    int d;
    if(drive < 0)
    {
        printf("\nPlease enter a drive number: ");
        scanf("%d", &d);
        printf("\nUsing %d drive\n",d);
    }
    else
    {
        d = drive;
        // printf("\nUsing %d drive\n",d);
    }
    set_primary_dev(d);
    if(try_fat() == 0)
    {
         return FAT_TYPE;
    }
    else if (try_ext() == 0)
    {
        return EXT_TYPE;
    }
    else
    {
        return UNKNOWN_DEVICE_FS;
    }
    
}

int try_fat()
{
    fl_init();
    if (fl_attach_media(secondary_storage_read, secondary_storage_write) != FAT_INIT_OK)
    {
        // printf("\033[1;31mERROR: Failed to init file system\n"); // Set text color to red

        return -1;
    }
    init_fs();
    return 0;
}

int try_ext()
{
    return -1; // EXT is not supported ATM
}