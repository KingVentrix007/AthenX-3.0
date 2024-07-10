#ifndef __ETHERNET__H
#define __ETHERNET__H

#include <stdint.h>
#define ETHERNET_ADDRESS_LENGTH 6
#define IP_ADDRESS_LENGTH       4

#define ETHERTYPE_ARP   0x0608  /* ARP ethertype 0x0806 but byte-swapped */
#define ETHERTYPE_IPv4  0x0008  /* IPv4 ethertype 0x0800 but byte-swapped */
#define ETHERNET_HEADER_SIZE    14
typedef struct Ethernet_Header
{
    uint8_t  destinationMAC[6];
    uint8_t  sourceMAC[6];
    uint16_t etherType;
    uint8_t  data[1];
} Ethernet_Header;
uint16_t SwapBytes16(uint16_t data);

uint32_t SwapBytes32(uint32_t data);
Ethernet_Header *EthernetCreatePacket(uint16_t etherType, uint16_t *pPacketSize, uint8_t *sourceMAC, void **dataPtr);
#endif