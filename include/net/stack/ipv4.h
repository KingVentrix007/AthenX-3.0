#ifndef __IPV4__H
#define __IPV4__H
#include <stdint.h>
#include "ip.h"

typedef struct {
    uint8_t ihl:4, version:4;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t src_ip[IP_ADDR_LEN];
    uint8_t dest_ip[IP_ADDR_LEN];
} __attribute__((packed)) ip_header_t;

#endif