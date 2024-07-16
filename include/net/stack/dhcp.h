#ifndef __DHCP__H
#define __DHCP__H

#include <stdint.h>

#define DHCP_CHADDR_LEN 16
#define DHCP_SNAME_LEN 64
#define DHCP_FILE_LEN 128
#define DHCP_OPTIONS_LEN 312

// Define the DHCP header struct
struct dhcp_header {
    uint8_t op;             // Message type: 1 for request, 2 for reply
    uint8_t htype;          // Hardware type
    uint8_t hlen;           // Hardware address length
    uint8_t hops;           // Hops
    uint32_t xid;           // Transaction ID
    uint16_t secs;          // Seconds elapsed
    uint16_t flags;         // Flags
    uint32_t ciaddr;        // Client IP address
    uint32_t yiaddr;        // 'Your' (client) IP address
    uint32_t siaddr;        // Next server IP address
    uint32_t giaddr;        // Relay agent IP address
    uint8_t chaddr[DHCP_CHADDR_LEN]; // Client hardware address
    char sname[DHCP_SNAME_LEN]; // Optional server host name
    char file[DHCP_FILE_LEN];   // Boot file name
    uint8_t options[DHCP_OPTIONS_LEN]; // Optional parameters
} __attribute__((packed));


void construct_dhcp_discover_packet(uint8_t *packet) ;
#endif