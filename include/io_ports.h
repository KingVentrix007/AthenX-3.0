#ifndef IO_PORTS_H
#define IO_PORTS_H

#include "types.h"

/**
 * read a byte from given port number
 */
uint8 inportb(uint16 port);

/**
 * write a given byte to given port number
 */
void outportb(uint16 port, uint8 val);

/**
 * read 2 bytes(short) from given port number
 */
uint16 inports(uint16 port);

/**
 * write given 2(short) bytes to given port number
 */
void outports(uint16 port, uint16 data);

/**
 * read 4 bytes(long) from given port number
 */
uint32 inportl(uint16 port);

/**
 * write given 4 bytes(long) to given port number
 */
void outportl(uint16 port, uint32 data);

void init_com1();

void configure_com1(uint16 baud_rate, uint8 data_bits, uint8 stop_bits, uint8 parity);
void write_to_com1(uint8 data);
void write_to_com1_string(char *s);
#endif
