#ifndef __UDP__H
#define __UDP__H


#include <stdint.h>
typedef struct UDP_Header
{
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;
    uint8_t data[1];
} UDP_Header;
#define UDP_HEADER_SIZE 8

#endif