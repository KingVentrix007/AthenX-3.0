#ifndef __ETHERNET__H
#define __ETHERNET__H
#include <stdint.h>
#include "mac.h"
#define ETHERNET_HEADER_SIZE 14
// Define the Ethernet header struct
typedef struct {
    uint8_t dest_mac[MAC_ADDR_LEN];
    uint8_t src_mac[MAC_ADDR_LEN];
    uint16_t ethertype;
} __attribute__((packed)) ethernet_header_t;
#endif