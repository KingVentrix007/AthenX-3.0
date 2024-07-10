#include <stdint.h>
#include <string.h>
#include "net/stack/dhcp.h"
#include "net/stack/ipv4.h"
#include "net/stack/ethernet.h"



void DHCP_Send_Discovery(uint8_t *sourceMAC)
{

    
    // uint8_t sourceMAC[6];
    // printf("sourceMAC ->");
    // for (size_t i = 0; i < sizeof(sourceMAC)/sizeof(sourceMAC[0]); i++)
    // {
    //         sourceMAC[i] = mac.bytes[i];
    //         printf("%0X:",sourceMAC[i]);
    // }
    // printf("\n");
    DHCP_HEADER *dhcpData;
    uint16_t packetSize = sizeof(DHCP_HEADER) - 76 + 8;
    Ethernet_Header *packet = UDP_Create_Packet(0xFFffFFff, 68, 67, &packetSize, sourceMAC, &dhcpData);

    dhcpData->opcode = DHCP_OPCODE_DISCOVER;
    dhcpData->hardwareType = DHCP_HARDWARE_ETHERNET;
    dhcpData->hardwareAddressLength = DHCP_ETHERNET_LENGTH;
    dhcpData->hops = 0;
    dhcpData->transactionID = 0x1234567; // TODO: SHould be random
    dhcpData->seconds = 0;
    dhcpData->flags = 0;
    dhcpData->clientIP = 0;
    dhcpData->yourIP = 0;
    dhcpData->serverIP = 0xFFffFFff;
    dhcpData->relayIP = 0;
    memcpy(dhcpData->clientHardwareAddress, sourceMAC, 6);
    memset(dhcpData->sname, 0, 64);
    memset(dhcpData->file, 0, 128);
    uint32_t magicCookie = SwapBytes32(DHCP_MAGIC_COOKIE);
    memcpy(dhcpData->options, &magicCookie, 4);
    // add the "DHCP Discover option - code - size (1) - value
    dhcpData->options[4] = DHCP_OPTION_MESSAGE_TYPE;
    dhcpData->options[5] = 1;
    dhcpData->options[6] = DHCP_MSG_TYPE_DISCOVER;
    dhcpData->options[7] = DHCP_OPTION_END;

    //printf("%d",packetSize);
    //printf("\n");

    // now send the packet
    printf("%0X:%0X:%0X:%0X:%0X:%0X\n",sourceMAC[0],sourceMAC[1],sourceMAC[2],sourceMAC[3],sourceMAC[4],sourceMAC[5]);
    EthernetSendPacket(packet, packetSize);
}