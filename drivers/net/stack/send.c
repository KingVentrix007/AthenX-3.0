#include "net/e1000.h"
#include "net/stack/dhcp.h"
#include "stdio.h"
#include "net/utils.h"
#include "net/stack/ethernet.h"
#include "net/stack/ip.h"
#include "net/stack/ipv4.h"
#include "net/stack/udp.h"
#include "net/network.h"
void create_dhcp_discover(uint8_t *packet, uint8_t *src_mac) ;
void process_received_packet();
int send_dhcp_request()
{
    uint8_t packet[512];
    memset(packet, 0, sizeof(packet));

    create_dhcp_discover(packet, mac);
    
}

// Define necessary constants and structures




uint16_t calculate_checksum(uint16_t *buf, int nwords) {
    uint32_t sum = 0;
    for (int i = 0; i < nwords; i++) {
        sum += buf[i];
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ~sum;
}

void create_dhcp_discover(uint8_t *packet, uint8_t *src_mac) {
    ethernet_header_t *eth_header = (ethernet_header_t *) packet;
    ip_header_t *ip_header = (ip_header_t *) (packet + ETHERNET_HEADER_SIZE);
    udp_header_t *udp_header = (udp_header_t *) (packet + ETHERNET_HEADER_SIZE + IP_HEADER_SIZE);
    dhcp_packet_t *dhcp_packet = (dhcp_packet_t *) (packet + ETHERNET_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);

    // Ethernet header
    memset(eth_header->dest_mac, 0xFF, MAC_ADDR_LEN);  // Broadcast
    memcpy(eth_header->src_mac, src_mac, MAC_ADDR_LEN);
    eth_header->ethertype = htons(0x0800);  // IP protocol

    // IP header
    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_header->tos = 0;
    ip_header->total_length = htons(IP_HEADER_SIZE + UDP_HEADER_SIZE + DHCP_HEADER_SIZE);
    ip_header->id = 0;
    ip_header->flags_fragment_offset = 0;
    ip_header->ttl = 64;
    ip_header->protocol = 17;  // UDP
    ip_header->checksum = 0;
    memset(ip_header->src_ip, 0, IP_ADDR_LEN);  // 0.0.0.0
    memset(ip_header->dest_ip, 0xFF, IP_ADDR_LEN);  // 255.255.255.255
    ip_header->checksum = calculate_checksum((uint16_t *) ip_header, IP_HEADER_SIZE / 2);

    // UDP header
    udp_header->src_port = htons(68);  // DHCP client port
    udp_header->dest_port = htons(67);  // DHCP server port
    udp_header->length = htons(UDP_HEADER_SIZE + DHCP_HEADER_SIZE);
    udp_header->checksum = 0;  // Optional

    // DHCP discover packet
    dhcp_packet->op = 1;  // Boot request
    dhcp_packet->htype = 1;  // Ethernet
    dhcp_packet->hlen = 6;  // MAC address length
    dhcp_packet->hops = 0;
    dhcp_packet->xid = htonl(0x12345678);  // Transaction ID
    dhcp_packet->secs = 0;
    dhcp_packet->flags = htons(0x8000);  // Broadcast flag
    memset(dhcp_packet->ciaddr, 0, IP_ADDR_LEN);
    memset(dhcp_packet->yiaddr, 0, IP_ADDR_LEN);
    memset(dhcp_packet->siaddr, 0, IP_ADDR_LEN);
    memset(dhcp_packet->giaddr, 0, IP_ADDR_LEN);
    memcpy(dhcp_packet->chaddr, src_mac, MAC_ADDR_LEN);
    memset(dhcp_packet->chaddr + MAC_ADDR_LEN, 0, 16 - MAC_ADDR_LEN);
    memset(dhcp_packet->sname, 0, 64);
    memset(dhcp_packet->file, 0, 128);
    dhcp_packet->magic_cookie = htonl(DHCP_MAGIC_COOKIE);

    // DHCP options
    uint8_t *options = dhcp_packet->options;
    options[0] = DHCP_OPTION_DISCOVER;
    options[1] = 1;
    options[2] = 1;  // DHCP Discover
    options[3] = DHCP_OPTION_END;

    // Calculate packet size
    int packet_size = ETHERNET_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE + DHCP_HEADER_SIZE;

    // Send the packet using e1000_transmit
    network_interface_t *info = get_current_netdev();
    info->transmit(packet, packet_size);
    // e1000_transmit(packet, packet_size);
}
