#ifndef __ETHERNET__H
#define __ETHERNET__H
#include <stdint.h>

// Define the Ethernet header struct
struct ethernet_header {
    uint8_t dest_mac[6];  // Destination MAC address
    uint8_t src_mac[6];   // Source MAC address
    uint16_t ethertype;   // Ethernet type (or length for IEEE 802.3)
} __attribute__((packed));

#endif