#ifndef __DEVICE__H
#define __DEVICE__H

#define AHCI_DEVICE  0x1
#define IDE_STORAGE_DEVICE  0x2
#define USB_DEVICE  0x3
#define VIRTUAL_DEVICE 0x4

#define UNKNOWN_DEVICE_FS 0x0
#define FAT_TYPE 0x1
#define NTFS_TYPE 0x2
#define EXT_TYPE 0x3


typedef struct
{
    int set;
    int storage_type;
    int storage_count; // Overall storage count
    int storage_specific_number; // drive number for specific storage device
    int file_system;

}pci_storage_device;

#endif