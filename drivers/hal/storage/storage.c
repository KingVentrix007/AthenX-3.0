#include "ide.h"
#include "ahci.h"
#include "pci_io.h"
#include "stddef.h"
#include "fat_filelib.h"
#include "fat_access.h"
#include "storage.h"
#include "stdlib.h"
#include "vfs.h"
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
    check_fs_types();
    printf("Storage devices initialized\n");
    
    
}

int check_fs_types()
{
    for (size_t i = 0; i < MAX_STORAGE_DEVICES; i++)
    {
        if(storage_devs[i].set == 0)
        {
            int ret = init_file_system(storage_devs[i].storage_count);
            storage_devs[i].file_system = ret;
        }
    }
}
int list_devices()
{
    for (size_t i = 0; i < MAX_STORAGE_DEVICES; i++)
    {
        if (storage_devs[i].set == 0)
        {
            char msg[40];
            uint32_t size = 0;
            if (storage_devs[i].storage_type == AHCI_DEVICE)
            {
                strcpy(msg, "AHCI device");
                size = get_ahci_sector_count(storage_devs[i].storage_specific_number);
            }
            else if (storage_devs[i].storage_type == IDE_STORAGE_DEVICE)
            {
                strcpy(msg, "IDE device");
                size = get_drive_size(storage_devs[i].storage_specific_number);
            }
            else
            {
                strcpy(msg, "Unknown device (High probability that it is supported by AHCI or IDE, but not guaranteed)");
                size = -1;
            }
            char *device_size = formatBytes(size * 512);
            printf("Device %d: %s - %u sectors %u bytes (%s) - File System: ", storage_devs[i].storage_count, msg, size, size * 512, device_size);
            print_fs_type(storage_devs[i].file_system);
            printf("\n");
        }
    }
    return 0;
}
void print_fs_type(int file_system) {
    switch(file_system) {
        case FAT_TYPE:
            printf("FAT");
            break;
        case NTFS_TYPE:
            printf("NTFS");
            break;
        case EXT_TYPE:
            printf("EXT");
            break;
        case UNKNOWN_DEVICE_FS:
        default:
            printf("\033[1;31mUNKNOWN\033[0m"); // Red color for UNKNOWN
            break;
    }
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

int get_primary_dev()
{
    return primary_dev.storage_count;
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
        // printf("device.set == %d\n", device.set);
        select_ahci_drive(device.storage_specific_number);
    }
    else
    {
        select_ide_drive(device.storage_specific_number);
    }
    
    fl_init();
    if (fl_attach_media(secondary_storage_read, secondary_storage_write) != FAT_INIT_OK)
    {
        // printf("\033[1;31mERROR: Failed to init file system\n"); // Set text color to red
        return -1;
    }
    // init_fs();

}

int secondary_storage_read(uint32 sector, uint8 *buffer, uint32 sector_count)
{
    if(primary_dev.storage_type == AHCI_DEVICE)
    {
        // printf_com("storage:: using AHCI");
        return ahci_read_sector_hal(sector,buffer,sector_count);
    }
    else
    {
        // printf_com("storage:: using ide\n");
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
uint64_t get_current_drive_sector_count()
{
    if(primary_dev.storage_type == AHCI_DEVICE)
    {
        return get_ahci_sector_count();
    }
}
#define SECTOR_SIZE 512
int write_iso_to_disk(uint8_t *iso_buffer, uint32_t iso_size) {
    uint32_t sector_count = (iso_size + SECTOR_SIZE - 1) / SECTOR_SIZE;

    for (uint32_t sector = 0; sector < sector_count; sector++) {
        uint8_t buffer[SECTOR_SIZE];
        memset(buffer, 0, SECTOR_SIZE);

        uint32_t offset = sector * SECTOR_SIZE;
        uint32_t bytes_to_write = (iso_size - offset) > SECTOR_SIZE ? SECTOR_SIZE : (iso_size - offset);
        memcpy(buffer, iso_buffer + offset, bytes_to_write);

        if (secondary_storage_write(sector, buffer, 1) != 1) {
            return -1; // Error writing to disk
        }
    }

    return 0; // Success
}
int install_athenx() {
    // Path to ISO file
    const char *iso_path = "/install.iso";
    // install_iso_to_disk()
    install_iso_to_disk(iso_path);
}
int check_iso_bootable(uint8_t *iso_buffer, uint32_t iso_size) {
    // Check for El Torito boot catalog (typically at sector 17, 2048 bytes offset)
    if (iso_size < 2048 * 18) {
        return 0; // Not enough data to check
    }

    // El Torito Boot Record volume descriptor at sector 17
    uint8_t *boot_catalog = iso_buffer + 2048 * 17;

    // Check for El Torito indicator
    if (memcmp(boot_catalog, "\x01\x0F\x00\x00", 4) == 0) {
        return 1; // Bootable ISO
    }
    
    return 0; // Not bootable
}
int install_mbr() {
    uint8_t mbr[SECTOR_SIZE] = {0};

    // Example simple MBR setup (replace with your MBR code if needed)
    // Boot code (first 446 bytes)
    uint8_t bootloader_code[446] = {
        // Your bootloader code goes here
        // This is a placeholder for the actual bootloader binary code
    };
    memcpy(mbr, bootloader_code, sizeof(bootloader_code));

    // Partition table (64 bytes)
    uint8_t partition_table[64] = {
        // Example partition entry (this is just a placeholder, configure as needed)
        0x80, 0x01, 0x01, 0x00, 0x83, 0xFE, 0x3F, 0x20, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
        // Add other partition entries here
    };
    memcpy(mbr + 446, partition_table, sizeof(partition_table));

    // Boot signature (0x55AA)
    mbr[510] = 0x55;
    mbr[511] = 0xAA;

    // Write MBR to the first sector
    if (secondary_storage_write(0, mbr, 1) != 1) {
        return -1; // Error writing MBR
    }

    return 0; // Success
}
int install_iso_to_disk(const char *iso_path) {
    // Open ISO file
    FILE *iso_file = fl_fopen(iso_path, "rb");
    if (iso_file == NULL) {
        printf("Error opening ISO file");
        return -1;
    }

    // Get ISO file size
    fl_fseek(iso_file, 0, SEEK_END);
    long iso_size = ftell(iso_file);
    fl_fseek(iso_file, 0, SEEK_SET);

    // Allocate buffer and read ISO contents
    uint8_t *iso_buffer = malloc(iso_size);
    if (iso_buffer == NULL) {
        printf("Error allocating memory for ISO buffer");
        fl_fclose(iso_file);
        return -1;
    }

    size_t read_size = fl_fread(iso_buffer, 1, iso_size, iso_file);
    if (read_size != iso_size) {
        printf("Error reading ISO file");
        free(iso_buffer);
        fl_fclose(iso_file);
        return -1;
    }

    fl_fclose(iso_file);

    // Check if ISO is bootable
    if (!check_iso_bootable(iso_buffer, iso_size)) {
        printf("The ISO file is not bootable\n");
        free(iso_buffer);
        return -1;
    }

    // Write ISO to disk
    if (write_iso_to_disk(iso_buffer, iso_size) != 0) {
        printf("Error writing ISO to disk\n");
        free(iso_buffer);
        return -1;
    }

    // Install MBR to make the disk bootable
    if (install_mbr() != 0) {
        printf("Error installing MBR\n");
        free(iso_buffer);
        return -1;
    }

    free(iso_buffer);
    printf("ISO installation completed successfully!\n");

    return 0;
}