#include "../include/fat32.h"
#include "../include/printf.h"
#include "../include/ide.h"
#include "../include/string.h"
// #include "../include/debug.h"
int init_fat_32(int disk)
{
   
    //fat_BS_t* fat_boot;
   
    //fat_extBS_32_t *fat_boot_ext_32;
    fat_BS_t BS;
    char buffer[512];
    memset(buffer,0,sizeof(buffer));
    ide_read_sectors(disk,1,0,(uint32)buffer);
    memcpy(&BS,buffer,sizeof(fat_BS_t));
    
    int total_sectors = (BS.total_sectors_16 == 0)? BS.total_sectors_32 : BS.total_sectors_16;
    char* label = (char*)(buffer + 54); // Offset for FAT12/FAT16
    if (strncmp(label, "FAT12   ", 8) == 0) {
        printf("FAT12 file system detected.\n");
    } else if (strncmp(label, "FAT16   ", 8) == 0) {
        printf("FAT16 file system detected.\n");
    } else {
        // If it's not FAT12 or FAT16, assume it's FAT32
        printf("FAT32 file system detected.\n");
        fat_extBS_32_t *extendedFATInfo = (fat_extBS_32_t *)(BS.extended_section); // Calculate the offset
        printf("Boot signature%x\n",extendedFATInfo->boot_signature);
        
       uint32_t root_cluster_32 = extendedFATInfo->root_cluster;
        uint32_t first_fat_sector = BS.reserved_sector_count;
        int fat_size = (BS.table_size_16 == 0)? extendedFATInfo->table_size_32 : BS.table_size_16;
        int root_dir_sectors = ((BS.root_entry_count * 32) + (BS.bytes_per_sector - 1)) / BS.bytes_per_sector;
        uint32_t data_sectors = BS.total_sectors_32 - (BS.reserved_sector_count + (BS.table_count * fat_size) + root_dir_sectors);
        int total_clusters = data_sectors / BS.sectors_per_cluster;
        int first_data_sector = BS.reserved_sector_count + (BS.table_count * fat_size) + root_dir_sectors;
        if(BS.sectors_per_cluster == 0)
        {
            printf("WARING\n");
        }
        else
        {
             int total_clusters = data_sectors / BS.sectors_per_cluster;
             if(total_clusters < 4085) 
                {
                    printf("FAT12\n");
                } 
                else if(total_clusters < 65525) 
                {
                    printf("FAT16\n");
                } 
                else
                {
                    printf("FAT32\n");
                }
        }
       int first_sector_of_cluster = ((root_cluster_32 - 2) * BS.sectors_per_cluster) + first_data_sector;
        
        // Call the readRootDirectory function to read and process the root directory
        //printf("%d\n",first_sector_of_cluster);
        printf("NAME: %s\n",extendedFATInfo->volume_label);
        readRootDirectory(disk, root_cluster_32, BS.bytes_per_sector, BS.sectors_per_cluster, first_data_sector);
        if(1 != 0)
        {

        }

    }
    printf("Total sectors: %d\n",total_sectors);
    //printf("%s\n",label);
    return total_sectors;
}
// void calculateFatInfo(fat_BS_t *fat_boot) {
//     FatInfo fatInfo;
//     fatInfo.totalSectors = (BS.total_sectors_16 == 0) ? BS.total_sectors_32 : BS.total_sectors_16;
//     fatInfo.fatSize = (BS.table_size_16 == 0) ? BS.table_size_16 : BS.table_size_16;
//     fatInfo.rootDirSectors = ((BS.root_entry_count * 32) + (BS.bytes_per_sector - 1)) / BS.bytes_per_sector;
//     fatInfo.firstDataSector = BS.reserved_sector_count + (BS.table_count * fatInfo.fatSize) + fatInfo.rootDirSectors;
//     fatInfo.firstFatSector = BS.reserved_sector_count;
//     fatInfo.dataSectors = fatInfo.totalSectors - (BS.reserved_sector_count + (BS.table_count * fatInfo.fatSize) + fatInfo.rootDirSectors);
//     fatInfo.totalClusters = fatInfo.dataSectors / BS.sectors_per_cluster;

//     // Print out the calculated values
//     printf("Total Sectors: %d\n", fatInfo.totalSectors);
//     printf("FAT Size: %d\n", fatInfo.fatSize);
//     printf("Root Directory Sectors: %d\n", fatInfo.rootDirSectors);
//     printf("First Data Sector: %d\n", fatInfo.firstDataSector);
//     printf("First FAT Sector: %d\n", fatInfo.firstFatSector);
//     printf("Data Sectors: %d\n", fatInfo.dataSectors);
//     printf("Total Clusters: %d\n", fatInfo.totalClusters);
// }
fat_BS_t *get_boot_sector_fat32(int disk)
{
//     FUNC_ADDR_NAME(&get_boot_sector_fat32,1,"i");
    
//     
//     fat_extBS_16_t *extendedFAT16 = (fat_extBS_16_t *)(buffer + sizeof(fat_BS_t)); // Calculate the offset
//     printf("Name: %s\n",extendedFATInfo->fat_type_label);
//     printf("Name2: %s\n",extendedFAT16->fat_type_label);
//     return BS;
}
// Define the DirectoryEntry struct here (previously provided)

// Function to read the root directory
void readRootDirectory(int disk, uint32_t root_cluster, uint16_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t first_fat_sector) {
    // Calculate the LBA of the root directory cluster
     uint32_t clusterOffset = (root_cluster - 2) * sectors_per_cluster;
    
    // Calculate the LBA of the root directory
    uint32_t rootDirLBA = first_fat_sector + clusterOffset;
    
    // Multiply by bytes per sector to get the absolute byte offset
    //rootDirLBA *= bytes_per_sector;

    // Read the root directory sectors into a buffer
    uint8_t root_dir_buffer[sectors_per_cluster * bytes_per_sector];
    memset(root_dir_buffer, 0, sizeof(root_dir_buffer));
    printf("root_dir_lba = %d\n",rootDirLBA);
    ide_read_sectors(disk, sectors_per_cluster, rootDirLBA, (uint32_t)root_dir_buffer);

    // Parse the directory entries
    for (int i = 0; i < sectors_per_cluster; i++) {
        // Iterate through sectors
        uint8_t* entry = root_dir_buffer + (i * bytes_per_sector);
        for (int j = 0; j < bytes_per_sector; j += sizeof(struct DirectoryEntry)) {
            struct DirectoryEntry* dir_entry = (struct DirectoryEntry*)(entry + j);

            // Check if the entry is valid (not deleted and not a long file name entry)
            if (dir_entry->name[0] != 0x00 && dir_entry->name[0] != 0xE5 && dir_entry->attributes != 0x0F) {
                // Extract and print the filename
                char filename[12]; // 8 characters for the name, 1 for '.', 3 for the extension, 1 for null terminator
                strncpy(filename, (char*)dir_entry->name, 8);
                filename[8] = '.';
                strncpy(filename + 9, (char*)(dir_entry->name + 8), 3);
                filename[12] = '\0'; // Null-terminate the string
                printf("Filename: %s\n", filename);
                printf("size %dB\n",dir_entry->file_size);
            }
        }
    }
}


//Experimental
// #include <stdio.h>
// #include <stdint.h>
// #include "../include/string.h"

// Define the FAT boot sector struct and the extended FAT32 struct
// (fat_BS_t and fat_extBS_32_t as defined in your previous messages)

// Struct to store the relevant FAT information
// typedef struct FatInfo {
//     fat_BS_t commonInfo;
//     fat_extBS_32_t extInfo;
//     char fatTypeLabel[8]; // To store the FAT type label
// } FatInfo;

// Function to read the FAT boot sector and extract relevant information
// Struct to store the relevant FAT information
// typedef struct FatInfo {
//     fat_BS_t commonInfo;
//     fat_extBS_32_t extInfo;
//     char fatTypeLabel[8]; // To store the FAT type label
// } FatInfo;

// Function to read the FAT boot sector and extract relevant information
// int readFatBootSector(uint8_t drive, uint32_t lba, uint32_t numLba, FatInfo *fatInfo) {
//     // Buffer to store the sector data
//     uint32_t buffer[128]; // Assuming sector size is 512 bytes, adjust if needed

//     // Read the FAT boot sector
//     if (ide_read_sectors(drive, lba, numLba, buffer) != 0) {
//         printf("Failed to read the FAT boot sector.\n");
//         return -1;
//     }

//     // Copy the common information into the FatInfo struct
//     memcpy(&fatInfo->commonInfo, buffer, sizeof(fat_BS_t));

//     // Copy the FAT type label from the boot code area (bytes 36-42)
//     memcpy(fatInfo->fatTypeLabel, &buffer[9], 8);

//     // Check if it's a FAT32 file system
//     if (strcmp(fatInfo->fatTypeLabel, "FAT32   ") == 0) {
//         // If it's FAT32, copy the extended information
//         memcpy(&fatInfo->extInfo, &buffer[sizeof(fat_BS_t)], sizeof(fat_extBS_32_t));
//     }

//     // Print the relevant information
//     printf("Common FAT Info:\n");
//     printf("Bytes Per Sector: %u\n", fatInfo->commonInfo.bytes_per_sector);
//     printf("Sectors Per Cluster: %u\n", fatInfo->commonInfo.sectors_per_cluster);
//     // Print other common fields

//     // Print total sectors and FAT type label
//     printf("Total Sectors: %u\n", (fatInfo->commonInfo.total_sectors_16 == 0) ?
//         fatInfo->commonInfo.total_sectors_32 : fatInfo->commonInfo.total_sectors_16);
//     printf("FAT Type Label: %s\n", fatInfo->fatTypeLabel);

//     if (strcmp(fatInfo->fatTypeLabel, "FAT32   ") == 0) {
//         printf("\nExtended FAT32 Info:\n");
//         printf("Table Size 32: %u\n", fatInfo->extInfo.table_size_32);
//         printf("Root Cluster: %u\n", fatInfo->extInfo.root_cluster);
//         // Print other extended fields specific to FAT32
//     }

//     return 0;
// }

// int init_fat_32(int disk) {
//     uint8_t drive = disk; // Example drive number
//     uint32_t lba = 0; // Example LBA address
//     uint32_t numLba = 1; // Number of LBAs to read
//     FatInfo fatInfo;

//     if (readFatBootSector(drive, lba, numLba, &fatInfo) == 0) {
//         printf("success");
//         return 1;
//         // Successfully read and processed the FAT boot sector
//     }

//     return 0;
// }
