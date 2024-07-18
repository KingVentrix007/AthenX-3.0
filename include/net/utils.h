#ifndef __UTILS_NET__H
#define __UTILS_NET__H
#include "stdint.h"
int is_little_endian(void);

uint16_t htons(uint16_t hostshort);

uint32_t htonl(uint32_t hostlong);

uint16_t ntohs(uint16_t netshort);

uint32_t ntohl(uint32_t netlong);

uint16_t calculate_checksum(uint16_t *buf, int nwords);
#endif