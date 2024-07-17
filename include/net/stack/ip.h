#ifndef __IP__H
#define __IP__H
#include "mac.h"
#include "stdint.h"
#define IP_ADDR_LEN 4
#define IP_HEADER_SIZE 20

#define HOSTNAME_LEN 64
typedef struct {
    uint8_t ip_address[IP_ADDR_LEN];
    uint8_t subnet_mask[IP_ADDR_LEN];
    uint8_t gateway[IP_ADDR_LEN];
    uint8_t dns_server[IP_ADDR_LEN]; // Optional
    uint8_t mac_address[MAC_ADDR_LEN];
    char hostname[HOSTNAME_LEN];
} network_info_t;
extern network_info_t net_info;
void wait_for_receive(int packet_type);
#endif