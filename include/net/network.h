#ifndef __NETWORK__H
#define __NETWORK__H
#include "net/stack/dhcp.h"
#include "net/stack/ipv4.h"
#include "net/stack/ethernet.h"
#include "net/stack/udp.h"
 struct {
        struct ethernet_header* eth;
        struct arp_header* arp;
        struct ip_header* ip;
        /* Can TCP and UDP be a union? */
        struct udp_header* udp;
    } hdr;

int send_dhcp_request();
#endif