#include "net/stack/dhcp.h"
#include "net/stack/ipv4.h"
#include "net/stack/ethernet.h"
#include "net/stack/udp.h"
#include "net/e1000.h"
#include "net/network.h"
void construct_dhcp_discover_packet(uint8_t *packet) {
    
    struct hdr * header = packet;
    // Fill Ethernet header
    memset(eth_hdr->dest_mac, 0xff, 6);  // Broadcast address
    // Fill src_mac with your MAC address
   memcpy(eth_hdr->src_mac, mac, 6);    // Source MAC address
    eth_hdr->ethertype = htons(0x0800);  // Ethertype for IP

    // Fill IP header
    ip_hdr->version_ihl = (4 << 4) | 5;  // IPv4 and header length
    ip_hdr->type_of_service = 0;
    ip_hdr->total_length = htons(sizeof(struct ip_header) + sizeof(struct udp_header) + sizeof(struct dhcp_header));
    ip_hdr->identification = htons(0);
    ip_hdr->flags_fragment_offset = htons(0x4000); // Don't fragment
    ip_hdr->ttl = 64;
    ip_hdr->protocol = 17; // UDP
    ip_hdr->header_checksum = 0; // Initially 0
    ip_hdr->src_ip = htonl(0x00000000); // 0.0.0.0
    ip_hdr->dest_ip = htonl(0xffffffff); // 255.255.255.255

    // Fill UDP header
    udp_hdr->src_port = htons(68); // DHCP client port
    udp_hdr->dest_port = htons(67); // DHCP server port
    udp_hdr->length = htons(sizeof(struct udp_header) + sizeof(struct dhcp_header));
    udp_hdr->checksum = 0; // Initially 0

    // Fill DHCP header
    dhcp_hdr->op = 1; // Request
    dhcp_hdr->htype = 1; // Ethernet
    dhcp_hdr->hlen = 6; // MAC address length
    dhcp_hdr->hops = 0;
    dhcp_hdr->xid = htonl(0x12345678); // Transaction ID
    dhcp_hdr->secs = htons(0);
    dhcp_hdr->flags = htons(0x8000); // Broadcast flag
    dhcp_hdr->ciaddr = htonl(0x00000000); // Client IP address
    dhcp_hdr->yiaddr = htonl(0x00000000); // 'Your' (client) IP address
    dhcp_hdr->siaddr = htonl(0x00000000); // Next server IP address
    dhcp_hdr->giaddr = htonl(0x00000000); // Relay agent IP address
    memset(dhcp_hdr->chaddr, 0, DHCP_CHADDR_LEN);
    // Fill chaddr with your MAC address
    memcpy(dhcp_hdr->chaddr, mac, 6); // Client hardware address
    memset(dhcp_hdr->sname, 0, DHCP_SNAME_LEN);
    memset(dhcp_hdr->file, 0, DHCP_FILE_LEN);

    // DHCP options (DHCP Discover)
    uint8_t *options = dhcp_hdr->options;
    options[0] = 0x63; // Magic cookie
    options[1] = 0x82;
    options[2] = 0x53;
    options[3] = 0x63;
    options[4] = 53; // DHCP Message Type
    options[5] = 1;  // Length
    options[6] = 1;  // DHCP Discover
    options[7] = 255; // End option
}

