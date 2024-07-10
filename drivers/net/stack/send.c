#include "stdint.h"
#include "net/e1000.h"
#include "net/stack/ethernet.h"
uint8_t packetBuffer[1024];

void send_dhcp_request() {
    uint8_t dhcp_request[300] = {};
    // uint8_t mac_address[6] = mac;
    uint32_t xid = 0x12345678;
    DHCP_Send_Discovery(mac);
}
Ethernet_Header *EthernetCreatePacket(uint16_t etherType, uint16_t *pPacketSize, uint8_t *sourceMAC, void **dataPtr)
{
    // add size of ethernet header to packet size
    *pPacketSize += ETHERNET_HEADER_SIZE;

    Ethernet_Header *packet = (Ethernet_Header *)packetBuffer;

    // TODO: Translate MAC from target IP
    packet->destinationMAC[0] = 0xFF;
    packet->destinationMAC[1] = 0xFF;
    packet->destinationMAC[2] = 0xFF;
    packet->destinationMAC[3] = 0xFF;
    packet->destinationMAC[4] = 0xFF;
    packet->destinationMAC[5] = 0xFF;

    // copy source MAC
    memcpy(packet->sourceMAC, sourceMAC, 6);

    packet->etherType = etherType;

    *dataPtr = packet->data;
    
    return packet;
}
void EthernetSendPacket(Ethernet_Header *packet, uint16_t dataSize)
{    
    // if (!NIC_Present)
    // {
    //     printf("ERROR: Can't send packet without a network card present!\n");
    //     return;
    // }

    // TODO: Support multiple NIC's and send this packet to the NIC owning the MAC in the packet
    sendPacket(packet, dataSize);
}

uint16_t SwapBytes16(uint16_t data)
{
    uint16_t returnVal;
    uint8_t *dst = (uint8_t *)&returnVal;
    uint8_t *src = (uint8_t *)&data;
    dst[0] = src[1];
    dst[1] = src[0];

    return returnVal;
}

// swap the bytes of a 32-bit value and return the result
uint32_t SwapBytes32(uint32_t data)
{
    uint32_t returnVal;
    uint8_t *dst = (uint8_t *)&returnVal;
    uint8_t *src = (uint8_t *)&data;
    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];

    return returnVal;
}