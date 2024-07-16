#ifndef __UDP__H
#define __UDP__H

#include <stdint.h>

// Define the UDP header struct
struct udp_header {
    uint16_t src_port;      // Source port
    uint16_t dest_port;     // Destination port
    uint16_t length;        // UDP length
    uint16_t checksum;      // UDP checksum
} __attribute__((packed));

#endif