#include "stdint.h"
#include "net/stack/udp.h"
#include "net/stack/ethernet.h"
Ethernet_Header *UDP_Create_Packet(uint32_t targetIP, uint16_t sourcePort, uint16_t destPort, uint16_t *pPacketSize, uint8_t *sourceMAC, void **dataPtr)
{
    uint16_t originalPacketSize = *pPacketSize;

    // increase packet size by 8 bytes for the UDP header
    *pPacketSize += UDP_HEADER_SIZE;
    UDP_Header *udpHeader;
    Ethernet_Header *header = IPv4_CreatePacket(targetIP, pPacketSize, sourceMAC, &udpHeader);

    udpHeader->sourcePort = SwapBytes16(sourcePort);
    udpHeader->destinationPort = SwapBytes16(destPort);
    udpHeader->length = SwapBytes16(originalPacketSize + UDP_HEADER_SIZE);
    udpHeader->checksum = 0;

    *dataPtr = udpHeader->data;

    return header;
}