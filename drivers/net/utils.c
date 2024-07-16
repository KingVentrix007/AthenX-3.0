#include <stdint.h>
#include "net/utils.h"
// Check if the system is little-endian or big-endian
int is_little_endian() {
    uint16_t test = 1;
    return *((uint8_t *)&test) == 1;
}

// Convert a 16-bit value from host byte order to network byte order
uint16_t htons(uint16_t hostshort) {
    if (is_little_endian()) {
        return (hostshort >> 8) | (hostshort << 8);
    } else {
        return hostshort;
    }
}

// Convert a 32-bit value from host byte order to network byte order
uint32_t htonl(uint32_t hostlong) {
    if (is_little_endian()) {
        return ((hostlong >> 24) & 0x000000FF) |
               ((hostlong >> 8) & 0x0000FF00) |
               ((hostlong << 8) & 0x00FF0000) |
               ((hostlong << 24) & 0xFF000000);
    } else {
        return hostlong;
    }
}

// Convert a 16-bit value from network byte order to host byte order
uint16_t ntohs(uint16_t netshort) {
    return htons(netshort);
}

// Convert a 32-bit value from network byte order to host byte order
uint32_t ntohl(uint32_t netlong) {
    return htonl(netlong);
}
