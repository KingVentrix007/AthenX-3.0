#include "net/stack/udp.h"
#include "net/stack/ethernet.h"
#include "net/stack/ipv4.h"
uint32_t clientIP = 0;  // our IP address
uint32_t gatewayIP; // IP address of the router
uint32_t DNS_Servers[4];
uint32_t subnetMask;    // honestly, I've never known what this is even for. Maybe I'll find out soon.

Ethernet_Header *IPv4_CreatePacket(uint32_t targetIP, uint16_t *pPacketSize, uint8_t *sourceMAC, void **dataPtr)
{
    // Increase packet size by 20 bytes for the IP header
    *pPacketSize += 20;
    uint16_t totalLength = *pPacketSize;
    IPv4_Header *ipHeader;
    Ethernet_Header *packet = EthernetCreatePacket(ETHERTYPE_IPv4, pPacketSize, sourceMAC, &ipHeader);
    
    ipHeader->version = 4;
    ipHeader->internetHeaderLength = IPv4_MINIMUM_HEADER_LENGTH / sizeof(uint32_t);
    ipHeader->typeOfService = 0;
    ipHeader->totalLength = SwapBytes16(totalLength);
    ipHeader->identification = 0;
    ipHeader->fragmentOffsetAndFlags = (0x0040);
    ipHeader->timeToLive = IPv4_TIME_TO_LIVE;
    ipHeader->protocol = IPv4_PROTO_UDP;
    ipHeader->headerChecksum = 0; // must be computed later
    ipHeader->sourceIP = clientIP; // TODO: Support multiple NIC's
    ipHeader->destinationIP = targetIP;
    
    // update header checksum
    IPv4_UpdateHeaderChecksum(ipHeader);

    /*for (int i = 0; i < 60; ++i)
    {
        printf("0x%x",((uint8_t*)ipHeader)[i]);
        printf(' ');
    }*/
    //printf("\n");

    *dataPtr = ipHeader->data;

    return packet;
}

void IPv4_UpdateHeaderChecksum(IPv4_Header *header)
{
    int bytesLeft = header->internetHeaderLength * 4;
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t *)header;

    while (bytesLeft > 1)
    {
        sum += *ptr++;
        bytesLeft -= 2;
    }

    //  Add left-over byte, if any
    if (bytesLeft > 0)
        sum += *(uint8_t *)ptr;

    //  Fold 32-bit sum to 16 bits
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    header->headerChecksum = ~((uint16_t)sum);
}