#ifndef __UDP__H
#define __UDP__H

#include <stdint.h>

// Define the UDP header struct

#define UDP_HEADER_SIZE 8







typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_header_t;
#endif