#include "ide.h"
#include "ahci.h"
#include "pci_io.h"
#include "stddef.h"
#include "fat_filelib.h"
#include "fat_access.h"
#include "storage.h"
#define MAX_STORAGE_DEVICES 100
pci_storage_device storage_devs[MAX_STORAGE_DEVICES];
pci_storage_device primary_dev;
int secondary_storage_read(uint32 sector, uint8 *buffer, uint32 sector_count);
int secondary_storage_write(uint32 sector, uint8 *buffer, uint32 sector_count);


int init_storage()
{
    for (size_t i = 0; i < MAX_STORAGE_DEVICES; i++)
    {
        storage_devs[i].set = -1;
    }
    ata_init();
    ahci_main();
    
}

int add_device(pci_storage_device dev)
{
    for (size_t i = 0; i < MAX_STORAGE_DEVICES; i++)
    {
        if(storage_devs[i].set != 0)
        {
            dev.storage_count = i;
            storage_devs[i] = dev;
            storage_devs[i].set = 0;
            return 0;
        }
    }
}
int set_primary_dev(int dev)
{
    pci_storage_device device = storage_devs[dev];
    if(device.set != 0)
    {
        printf("Device %d not set\n", dev);
        return -1;
    }
    primary_dev = device;
    if(device.storage_type == AHCI_DEVICE)
    {
        printf("device.set == %d\n", device.set);
        select_ahci_drive(device.storage_specific_number);
    }
    else
    {
        printf("ONLY one NON AHCI device can be used\n");
    }
    fl_init();
    // init_fs();

}

int secondary_storage_read(uint32 sector, uint8 *buffer, uint32 sector_count)
{
    if(primary_dev.storage_type == AHCI_DEVICE)
    {
        return ahci_read_sector_hal(sector,buffer,sector_count);
    }
    else
    {
        return ide_read_sectors_fat(sector,buffer,sector_count);
    }
}
int secondary_storage_write(uint32 sector, uint8 *buffer, uint32 sector_count)
{
    if(primary_dev.storage_type == AHCI_DEVICE)
    {
        return ahci_write_sector_hal(sector,buffer,sector_count);
    }
    else
    {
        return ide_write_sectors_fat(sector,buffer,sector_count);
    }
}