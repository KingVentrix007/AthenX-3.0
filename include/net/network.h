#ifndef __NETWORK__H
#define __NETWORK__H
#include "net/stack/dhcp.h"
#include "net/stack/ipv4.h"
#include "net/stack/ethernet.h"
#include "net/stack/udp.h"
#include "net/stack/ip.h"
typedef struct
{

    char name[100];
    network_info_t *info;
    int (*transmit)(char* buffer, uint32_t size);
    int (*receive)(char* buffer, uint32_t size);


}network_interface_t;
int send_dhcp_request();
int register_iface(network_interface_t* iface,char iface_name[100],int (*transmit_func)(char* buffer, uint32_t size),int (*receive_func)(char* buffer, uint32_t size));
#endif