#ifndef __FAT__32__
#define __FAT__32__
#include "../include/types.h"
#include "stdint.h"
typedef struct fat_extBS_32
{
	//extended fat32 stuff
	unsigned int		table_size_32;
	unsigned short		extended_flags;
	unsigned short		fat_version;
	unsigned int		root_cluster;
	unsigned short		fat_info;
	unsigned short		backup_BS_sector;
	unsigned char 		reserved_0[12];
	unsigned char		drive_number;
	unsigned char 		reserved_1;
	unsigned char		boot_signature;
	unsigned int 		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
}__attribute__((packed)) fat_extBS_32_t;
 
typedef struct fat_extBS_16
{
	//extended fat12 and fat16 stuff
	unsigned char		bios_drive_num;
	unsigned char		reserved1;
	unsigned char		boot_signature;
	unsigned int		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
}__attribute__((packed)) fat_extBS_16_t;
 
typedef struct fat_BS
{
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short 	        bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
 
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	unsigned char		extended_section[54];
 
}__attribute__((packed)) fat_BS_t;

typedef struct DirectoryEntry {
    uint8_t name[11];          // 8.3 format filename (8 bytes for name, 3 bytes for extension)
    uint8_t attributes;        // File attributes (e.g., hidden, read-only)
    uint8_t reserved1;         // Reserved for use by Windows NT
    uint8_t creation_time_tenths; // Millisecond timestamp at file creation
    uint16_t creation_time;    // Time file was created
    uint16_t creation_date;    // Date file was created
    uint16_t access_date;      // Date of last access
    uint16_t high_cluster;     // High 16 bits of the first cluster address
    uint16_t modification_time; // Time of last modification
    uint16_t modification_date; // Date of last modification
    uint16_t low_cluster;      // Low 16 bits of the first cluster address
    uint32_t file_size;        // File size in bytes
    // Additional fields for long filename support (optional)
    uint8_t long_name[13];     // Long filename characters (UTF-16 encoded)
    uint8_t long_name_order;   // Long filename entry order (0x40 for the last, 0x00 for others)
};
// typedef struct FatInfo {
//     int totalSectors;
//     int fatSize;
//     int rootDirSectors;
//     int firstDataSector;
//     int firstFatSector;
//     int dataSectors;
//     int totalClusters;
// } FatInfo;
int init_fat_32(int disk);
fat_BS_t *get_boot_sector_fat32(int disk);
void readRootDirectory(int disk, uint32_t root_cluster, uint16_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t reserved_sector_count);
#endif