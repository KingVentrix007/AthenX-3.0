#ifndef __IPV4__H
#define __IPV4__H
#include <stdint.h>
#include "net/stack/ethernet.h"
#define IP_VERSION_4    4
#define IPv4_MINIMUM_HEADER_LENGTH  20
#define IPv4_TIME_TO_LIVE   64
#define IPv4_PROTO_UDP      0x11
typedef struct ARP_IPv4_Packet
{
    uint8_t  sourceMAC[6];
    uint32_t sourceIP;
    uint8_t  destinationMAC[6];
    uint32_t destinationIP;
} ARP_IPv4_Packet;
typedef struct IPv4_Header
{
    uint8_t internetHeaderLength : 4; // header-length is number of 32-bit words, usually this will be five
    uint8_t version : 4;
    uint8_t typeOfService;
    uint16_t totalLength; // header and data
    uint16_t identification;
    //uint16_t flags : 3;
    //uint16_t fragmentOffset : 13;
    uint16_t fragmentOffsetAndFlags;
    uint8_t timeToLive;
    uint8_t protocol;
    uint16_t headerChecksum;
    uint32_t sourceIP;
    uint32_t destinationIP;
    uint8_t data[1];
} IPv4_Header;
Ethernet_Header *IPv4_CreatePacket(uint32_t targetIP, uint16_t *pPacketSize, uint8_t *sourceMAC, void **dataPtr);
#endif