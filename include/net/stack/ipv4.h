#ifndef __IPV4__H
#define __IPV4__H
#include <stdint.h>

// Define the IPv4 header struct
struct ip_header {
    uint8_t version_ihl;        // Version (4 bits) + Internet header length (4 bits)
    uint8_t type_of_service;    // Type of service 
    uint16_t total_length;      // Total length 
    uint16_t identification;    // Identification
    uint16_t flags_fragment_offset; // Flags (3 bits) + Fragment offset (13 bits)
    uint8_t ttl;                // Time to live
    uint8_t protocol;           // Protocol
    uint16_t header_checksum;   // Header checksum
    uint32_t src_ip;            // Source IP address
    uint32_t dest_ip;           // Destination IP address
} __attribute__((packed));


#endif