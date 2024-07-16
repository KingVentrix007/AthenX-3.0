#include "stdint.h"
#include "net/stack/udp.h"
#include "net/stack/ethernet.h"
#include "net/stack/dhcp.h"

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

void UDP_ProcessReceivedPacket(UDP_Header *packet, uint8_t *sourceMAC)
{
    uint16_t size = SwapBytes16(packet->length);
    uint16_t sourcePort = SwapBytes16(packet->sourcePort);
    uint16_t destinationPort = SwapBytes16(packet->destinationPort);
    
    if (1==0)
    {
        printf("UDP packet with ");
        printf("%d",size);
        printf(" bytes received.\n");
    }

    if (destinationPort == 68)
    {
        if(1==0)
            printf("DHCP Packet received\n");
        DHCP_ProcessReply((DHCP_HEADER *)packet->data, sourceMAC);
    }

    // if (sourcePort == TFTP_PORT)
    // {
    //     TFTP_ProcessPacket((TFTP_Header *)packet->data, sourcePort, destinationPort, size - UDP_HEADER_SIZE, sourceMAC);
    // }
}