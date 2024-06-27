#include "storage.h"
#include "fat_access.h"
#include "scanf.h"
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
    fl_init();
    if (fl_attach_media(secondary_storage_read, secondary_storage_write) != FAT_INIT_OK)
    {
        printf("\033[1;31mERROR: Failed to init file system\n"); // Set text color to red
        return -1;
    }
    init_fs();
    
    return 0; // Assuming a return value is needed at the end of the function
}