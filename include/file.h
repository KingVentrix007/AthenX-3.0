#ifndef __FILE__H
#define __FILE__H
#include "stdint-gcc.h"
#include "stdbool.h"
#include "stddef.h"
#define STDERR_VALUE 0x3241
#define STDOUT_VALUE 0x5674
#define STDIN_VALUE  0x8765

#define FAT32 0x1
#define EXT2  0x2
#define EXT3 0x3
#define EXT4 0x4

typedef struct drive_info
{
    int drive_num;
    char drive_letter;
    char drive_name[260];
    uint32_t drive_type;
    size_t drive_size;
    bool readonly;


};


#endif