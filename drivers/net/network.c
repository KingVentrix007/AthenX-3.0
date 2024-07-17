#include "net/network.h"
#include "net/stack/ip.h"

int request_ip_and_wait()
{
    send_dhcp_request();

    wait_for_receive(2);
}

int register_iface(network_interface_t* iface, char iface_name[100], int (*transmit_func)(char* buffer, uint32_t size), int (*receive_func)(char* buffer, uint32_t size)) {
    // Copy the interface name
    strncpy(iface->name, iface_name, sizeof(iface->name) - 1);
    iface->name[sizeof(iface->name) - 1] = '\0';  // Ensure null-termination

    // Assign the transmit and receive function pointers
    iface->transmit = transmit_func;
    iface->receive = receive_func;

    // Wait for a DHCP ACK packet
   request_ip_and_wait();

    // Get IP, MAC, and other information from the net_info struct
    memcpy(iface->info->ip_address, net_info.ip_address, IP_ADDR_LEN);
    memcpy(iface->info->subnet_mask, net_info.subnet_mask, IP_ADDR_LEN);
    memcpy(iface->info->gateway, net_info.gateway, IP_ADDR_LEN);
    memcpy(iface->info->dns_server, net_info.dns_server, IP_ADDR_LEN);
    memcpy(iface->info->mac_address, net_info.mac_address, MAC_ADDR_LEN);
    strncpy(iface->info->hostname, net_info.hostname, HOSTNAME_LEN);
    iface->info->hostname[HOSTNAME_LEN - 1] = '\0';  // Ensure null-termination

    return 0; // Return 0 on success
}
