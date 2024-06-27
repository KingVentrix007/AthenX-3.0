#ifndef __STORAGE__H
#define __STORAGE__H
#include "ahci.h"
#include "pci_dev.h"
int add_device(pci_storage_device dev);
int init_storage();

int secondary_storage_read(uint32 sector, uint8 *buffer, uint32 sector_count);
int secondary_storage_write(uint32 sector, uint8 *buffer, uint32 sector_count);

int set_primary_dev(int d);
#endif