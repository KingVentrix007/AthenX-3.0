#include "stdint.h"
#include "net/e1000.h"
void send_dhcp_request() {
    uint8_t dhcp_request[300] = {};
    // uint8_t mac_address[6] = mac;
    uint32_t xid = 0x12345678;
    create_dhcp_request(dhcp_request, mac, xid);
    udp_send_packet((uint8_t*)"\xff\xff\xff\xff", 53, dhcp_request, sizeof(dhcp_request));
}
void udp_send_packet(uint8_t* ip, uint16_t port, uint8_t* payload, uint16_t payload_size) {
    uint16_t udp_length = 8 + payload_size;
    uint8_t udp_packet[udp_length];
    udp_packet[0] = 0;  // Source port (68 for DHCP client)
    udp_packet[1] = 68;
    udp_packet[2] = port >> 8;  // Destination port
    udp_packet[3] = port & 0xFF;
    udp_packet[4] = udp_length >> 8;  // Length
    udp_packet[5] = udp_length & 0xFF;
    udp_packet[6] = 0;  // Checksum (initially zero)
    udp_packet[7] = 0;

    memcpy(udp_packet + 8, payload, payload_size);

    // Calculate checksum here if needed

    ip_send_packet((uint8_t*)"\xff\xff\xff\xff", 17, udp_packet, udp_length);  // 17 is the protocol number for UDP
}

void ip_send_packet(uint8_t* dest_ip, uint8_t protocol, uint8_t* payload, uint16_t payload_size) {
    uint16_t ip_length = 20 + payload_size;
    uint8_t ip_packet[ip_length];
    ip_packet[0] = 0x45;  // Version and header length
    ip_packet[1] = 0;  // Type of Service
    ip_packet[2] = ip_length >> 8;
    ip_packet[3] = ip_length & 0xFF;
    ip_packet[4] = 0;  // ID (initially zero)
    ip_packet[5] = 0;
    ip_packet[6] = 0;  // Flags and fragment offset
    ip_packet[7] = 0;
    ip_packet[8] = 64;  // TTL
    ip_packet[9] = protocol;  // Protocol
    ip_packet[10] = 0;  // Checksum (initially zero)
    ip_packet[11] = 0;

    uint8_t source_ip[4] = {0, 0, 0, 0};  // Source IP for DHCP request
    memcpy(ip_packet + 12, source_ip, 4);  // Source IP
    memcpy(ip_packet + 16, dest_ip, 4);  // Destination IP

    // Calculate IP checksum here

    memcpy(ip_packet + 20, payload, payload_size);

    ethernet_send_packet(dest_ip, 0x0800, ip_packet, ip_length);  // 0x0800 is the type for IPv4
}

void ethernet_send_packet(uint8_t* dest_ip, uint16_t ether_type, uint8_t* payload, uint16_t payload_size) {
    uint16_t frame_length = 14 + payload_size;
    uint8_t frame[frame_length];
    memset(frame, 0xFF, 6);  // Destination MAC (broadcast)
    memcpy(frame + 6, mac, 6);  // Source MAC
    frame[12] = ether_type >> 8;
    frame[13] = ether_type & 0xFF;

    memcpy(frame + 14, payload, payload_size);

    sendPacket(frame, frame_length);
}
