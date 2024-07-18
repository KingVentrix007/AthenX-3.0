#include "net/network.h"
#include "net/stack/ip.h"
#include "types.h"
#include "string.h"
#include "net/e1000.h" 
#define MAX_INTERFACES 10  // Example: Maximum number of interfaces

network_interface_t interfaces[MAX_INTERFACES];
network_interface_t *current_netdev = NULL;
int num_interfaces = 0;  // Track the number of registered interfaces

void print_mac_address(uint8_t mac[6]) {
    printf("   Mac Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
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
    // print_mac_address(iface->info->mac_address);
    // print_mac_address(net_info.mac_address);
    strncpy(iface->info->hostname, net_info.hostname, HOSTNAME_LEN);
    iface->info->hostname[HOSTNAME_LEN - 1] = '\0';  // Ensure null-termination
    interfaces[dev] = *iface;
    printf("Interface %d: %s\n", dev + 1, iface->name);
    printf("   IP Address: %d.%d.%d.%d\n", iface->info->ip_address[0], iface->info->ip_address[1], iface->info->ip_address[2], iface->info->ip_address[3]);
    // Print other details as needed (subnet mask, gateway, MAC address, hostname, etc.)
    // 
    print_mac_address(iface->info->mac_address);
    printf("   Subnet Mask: %d.%d.%d.%d\n", iface->info->subnet_mask[0], iface->info->subnet_mask[1], iface->info->subnet_mask[2], iface->info->subnet_mask[3]);
    printf("   Gateway: %d.%d.%d.%d\n", iface->info->gateway[0], iface->info->gateway[1], iface->info->gateway[2], iface->info->gateway[3]);
    printf("   DNS Server: %d.%d.%d.%d\n", iface->info->dns_server[0], iface->info->dns_server[1], iface->info->dns_server[2], iface->info->dns_server[3]);
    printf("   Hostname: %s\n", iface->info->hostname);
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

void print_all_network_devs() {
    printf("List of Registered Network Devices:\n");

    for (int i = 0; i < num_interfaces; i++) {
        network_interface_t *iface = &interfaces[0];
        
        printf("\n");
    }
}