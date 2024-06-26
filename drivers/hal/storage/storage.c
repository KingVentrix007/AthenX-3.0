#include "ide.h"
#include "ahci.h"
#include "pci_io.h"
#include "stddef.h"
#define MAX_STORAGE_DEVICES 100
pci_storage_device storage_devs[MAX_STORAGE_DEVICES];
pci_storage_device primary_dev;



int init_storage()
{
    for (size_t i = 0; i < MAX_STORAGE_DEVICES; i++)
    {
        storage_devs[i].set = -1;
    }
    
}

int add_device(pci_storage_device dev)
{
    for (size_t i = 0; i < MAX_STORAGE_DEVICES; i++)
    {
        if(storage_devs[i].set == -1)
        {
            storage_devs[i] = dev;
        }
    }
}
int set_primary_dev()
{
    
}