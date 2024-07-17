#include "net/network.h"
#include "net/stack/ip.h"
#include "types.h"
#define MAX_INTERFACES 10  // Example: Maximum number of interfaces

network_interface_t interfaces[MAX_INTERFACES];
network_interface_t *current_netdev = NULL;
int num_interfaces = 0;  // Track the number of registered interfaces
int request_ip_and_wait(int dev)
{
    send_dhcp_request();
    network_interface_t *iface = &interfaces[dev];
    wait_for_receive(2);
    memcpy(iface->info->ip_address, net_info.ip_address, IP_ADDR_LEN);
    memcpy(iface->info->subnet_mask, net_info.subnet_mask, IP_ADDR_LEN);
    memcpy(iface->info->gateway, net_info.gateway, IP_ADDR_LEN);
    memcpy(iface->info->dns_server, net_info.dns_server, IP_ADDR_LEN);
    memcpy(iface->info->mac_address, net_info.mac_address, MAC_ADDR_LEN);
    strncpy(iface->info->hostname, net_info.hostname, HOSTNAME_LEN);
    iface->info->hostname[HOSTNAME_LEN - 1] = '\0';  // Ensure null-termination
}
void set_current_netdev(network_interface_t *iface) {
    current_netdev = iface;
}
network_interface_t *get_current_netdev() {
    return current_netdev;
}
int register_iface(network_interface_t* iface, char iface_name[100], int (*transmit_func)(char* buffer, uint32_t size), int (*receive_func)(char* buffer, uint32_t size)) {
    // Copy the interface name
    strncpy(iface->name, iface_name, sizeof(iface->name) - 1);
    iface->name[sizeof(iface->name) - 1] = '\0';  // Ensure null-termination

    // Assign the transmit and receive function pointers
    iface->transmit = transmit_func;
    iface->receive = receive_func;

    // Wait for a DHCP ACK packet

    // Get IP, MAC, and other information from the net_info struct
   
    interfaces[num_interfaces] = *iface;
    num_interfaces++;
    if(strcmp(iface->name, "eth0") == 0) {
        set_current_netdev(iface);
    }
    request_ip_and_wait(num_interfaces-1);
    return 0; // Return 0 on success
}
