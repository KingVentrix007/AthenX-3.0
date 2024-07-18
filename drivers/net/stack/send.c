#include "net/e1000.h"
#include "net/stack/dhcp.h"
#include "stdio.h"
#include "net/utils.h"
#include "net/stack/ethernet.h"
#include "net/stack/ip.h"
#include "net/stack/ipv4.h"
#include "net/stack/udp.h"
#include "net/network.h"
void process_received_packet();
int send_dhcp_request()
{
    uint8_t packet[512];
    memset(packet, 0, sizeof(packet));

    int packet_size = create_dhcp_discover(packet, mac);
    e1000_transmit(packet, packet_size);
    
}

// Define necessary constants and structures




