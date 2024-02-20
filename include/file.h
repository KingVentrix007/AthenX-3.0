#ifndef __FILE__H
#define __FILE__H
#include "stdint-gcc.h"
#include "stdbool.h"
#include "stddef.h"
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