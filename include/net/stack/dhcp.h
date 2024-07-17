#ifndef __DHCP__H
#define __DHCP__H

#include <stdint.h>
#include "ip.h"
#define DHCP_HEADER_SIZE 240
#define DHCP_MAGIC_COOKIE 0x63825363
#define DHCP_OPTION_DISCOVER 53
#define DHCP_OPTION_END 255
typedef struct {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint8_t ciaddr[IP_ADDR_LEN];
    uint8_t yiaddr[IP_ADDR_LEN];
    uint8_t siaddr[IP_ADDR_LEN];
    uint8_t giaddr[IP_ADDR_LEN];
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    uint32_t magic_cookie;
    uint8_t options[312];  // Options are variable length
} __attribute__((packed)) dhcp_packet_t;
#endif