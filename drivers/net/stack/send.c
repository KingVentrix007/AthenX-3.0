#include "net/e1000.h"
#include "net/stack/dhcp.h"


int send_dhcp_request()
{
    uint8_t packet[1500];
    memset(packet, 0, sizeof(packet));

    construct_dhcp_discover_packet(packet);
    e1000_transmit(packet, sizeof(packet));
    
}

