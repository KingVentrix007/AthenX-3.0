#include "net/e1000.h"
#include "net/stack/dhcp.h"
#include "stdio.h"
#include "net/utils.h"
#include "net/stack/ethernet.h"
#include "net/stack/ipv4.h"
#include "net/stack/udp.h"
#include "net/stack/ip.h"
network_info_t net_info;
void process_received_packet() {
    char buffer[PACKET_SIZE];
    int length = e1000_receive(buffer, PACKET_SIZE);

    if (length < 0) {
        // No packet received
        return;
    }

    // Parse Ethernet header
    ethernet_header_t *eth_header = (ethernet_header_t *) buffer;
    if (ntohs(eth_header->ethertype) != 0x0800) {
        printf("Not an IP packet\n");
        return;
    }

    // Parse IP header
    ip_header_t *ip_header = (ip_header_t *) (buffer + ETHERNET_HEADER_SIZE);
    if (ip_header->protocol != 17) {
        printf("Not a UDP packet\n");
        return;
    }

    // Parse UDP header
    udp_header_t *udp_header = (udp_header_t *) (buffer + ETHERNET_HEADER_SIZE + IP_HEADER_SIZE);
    printf("UDP Source Port: %d\n", ntohs(udp_header->src_port));
    printf("UDP Destination Port: %d\n", ntohs(udp_header->dest_port));

    if (ntohs(udp_header->dest_port) != 68 || ntohs(udp_header->src_port) != 67) {
        printf("Not a DHCP response\n");
        return;
    }

    // Parse DHCP packet
    dhcp_packet_t *dhcp_packet = (dhcp_packet_t *) (buffer + ETHERNET_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);

    // Verify DHCP magic cookie
    if (ntohl(dhcp_packet->magic_cookie) != DHCP_MAGIC_COOKIE) {
        printf("Not a valid DHCP packet\n");
        return;
    }

    // Check DHCP options
    uint8_t *options = dhcp_packet->options;
    int i = 0;
    while (i < 312 && options[i] != DHCP_OPTION_END) {
        uint8_t option_type = options[i++];
        uint8_t option_length = options[i++];

        if (option_type == 53 && option_length == 1) {
            uint8_t dhcp_message_type = options[i];
            if (dhcp_message_type == 2) {
                // DHCP offer received
                printf("DHCP offer received\n");

                // Extract offered IP address
                uint8_t offered_ip[IP_ADDR_LEN];
                memcpy(offered_ip, dhcp_packet->yiaddr, IP_ADDR_LEN);
                printf("Offered IP: %d.%d.%d.%d\n",
                       offered_ip[0], offered_ip[1], offered_ip[2], offered_ip[3]);

                // You can now proceed with sending a DHCP request, etc.
                return;
            } else if (dhcp_message_type == 5) {
                // DHCP ACK received
                printf("DHCP ACK received\n");

                // Extract assigned IP address
                memcpy(net_info.ip_address, dhcp_packet->yiaddr, IP_ADDR_LEN);
                printf("Assigned IP: %d.%d.%d.%d\n",
                       net_info.ip_address[0], net_info.ip_address[1], net_info.ip_address[2], net_info.ip_address[3]);

                // Extract additional options if needed (subnet mask, gateway, etc.)
                // Example for subnet mask option (option 1) and gateway (option 3):
                for (int j = 0; j < 312 && options[j] != DHCP_OPTION_END; j++) {
                    if (options[j] == 1 && options[j+1] == IP_ADDR_LEN) {
                        memcpy(net_info.subnet_mask, &options[j+2], IP_ADDR_LEN);
                        printf("Subnet Mask: %d.%d.%d.%d\n",
                               net_info.subnet_mask[0], net_info.subnet_mask[1], net_info.subnet_mask[2], net_info.subnet_mask[3]);
                    }
                    if (options[j] == 3 && options[j+1] == IP_ADDR_LEN) {
                        memcpy(net_info.gateway, &options[j+2], IP_ADDR_LEN);
                        printf("Gateway: %d.%d.%d.%d\n",
                               net_info.gateway[0], net_info.gateway[1], net_info.gateway[2], net_info.gateway[3]);
                    }
                    if (options[j] == 6 && options[j+1] == IP_ADDR_LEN) {
                        memcpy(net_info.dns_server, &options[j+2], IP_ADDR_LEN);
                        printf("DNS Server: %d.%d.%d.%d\n",
                               net_info.dns_server[0], net_info.dns_server[1], net_info.dns_server[2], net_info.dns_server[3]);
                    }
                }

                // Extract MAC address from the DHCP packet (assuming it's the same as the client's MAC address)
                memcpy(net_info.mac_address, dhcp_packet->chaddr, MAC_ADDR_LEN);
                printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                       net_info.mac_address[0], net_info.mac_address[1], net_info.mac_address[2],
                       net_info.mac_address[3], net_info.mac_address[4], net_info.mac_address[5]);

                // Optionally set hostname if available
                strncpy(net_info.hostname, (char*) dhcp_packet->sname, HOSTNAME_LEN);
                net_info.hostname[HOSTNAME_LEN - 1] = '\0';  // Ensure null-termination
                printf("Hostname: %s\n", net_info.hostname);

                // You can now configure the network interface with the assigned IP, subnet mask, gateway, etc.
                return;
            }
        }
        i += option_length;
    }
}