#include <stdint.h>
#include <string.h>

#define DHCP_REQUEST 3
#define DHCP_MAGIC_COOKIE 0x63825363
uint32_t htonl(uint32_t hostlong) {
    return ((hostlong & 0xff000000) >> 24) |
           ((hostlong & 0x00ff0000) >> 8) |
           ((hostlong & 0x0000ff00) << 8) |
           ((hostlong & 0x000000ff) << 24);
}
void create_dhcp_request(uint8_t *packet, uint8_t *mac_address, uint32_t xid) {
    memset(packet, 0, 240);  // Zero the DHCP header

    // DHCP header
    packet[0] = 1;  // op: BOOTREQUEST
    packet[1] = 1;  // htype: Ethernet
    packet[2] = 6;  // hlen: MAC length
    packet[3] = 0;  // hops
    *(uint32_t *)(packet + 4) = xid;  // xid: Transaction ID
    packet[8] = 0;  // secs
    packet[9] = 0;
    packet[10] = 0;  // flags
    packet[11] = 0;
    // ciaddr: Client IP address (0.0.0.0)
    // yiaddr: Your IP address (0.0.0.0)
    // siaddr: Server IP address (0.0.0.0)
    // giaddr: Gateway IP address (0.0.0.0)
    memcpy(packet + 28, mac_address, 6);  // chaddr: Client hardware address
    // sname: Server host name (optional, zeroed)
    // file: Boot file name (optional, zeroed)

    // DHCP options
    uint8_t *options = packet + 240;
    *(uint32_t *)options = htonl(DHCP_MAGIC_COOKIE);  // Magic cookie
    options += 4;

    // Option 53: DHCP Message Type (1 byte for type, 1 byte for length, 1 byte for value)
    options[0] = 53;
    options[1] = 1;
    options[2] = DHCP_REQUEST;
    options += 3;

    // Option 50: Requested IP Address (1 byte for type, 1 byte for length, 4 bytes for IP address)
    options[0] = 50;
    options[1] = 4;
    // For demonstration, request IP address 192.168.1.100
    options[2] = 192;
    options[3] = 168;
    options[4] = 1;
    options[5] = 100;
    options += 6;

    // Option 55: Parameter Request List (1 byte for type, 1 byte for length, list of parameter codes)
    options[0] = 55;
    options[1] = 4;  // Request 4 parameters
    options[2] = 1;  // Subnet Mask
    options[3] = 3;  // Router
    options[4] = 6;  // Domain Name Server
    options[5] = 15;  // Domain Name
    options += 6;

    // Option 54: Server Identifier (1 byte for type, 1 byte for length, 4 bytes for server IP address)
    options[0] = 54;
    options[1] = 4;
    // For demonstration, use server IP address 192.168.1.1
    options[2] = 192;
    options[3] = 168;
    options[4] = 1;
    options[5] = 1;
    options += 6;

    // Option 255: End (1 byte for type)
    options[0] = 255;
}
