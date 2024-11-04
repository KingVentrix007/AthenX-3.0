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



void configure_com1(uint16 baud_rate);
void write_to_com1(uint8 data);
void write_to_com1_string(const char *s);
void printf_com(const char* format, ...);
void cpuid(unsigned int code, unsigned int *a, unsigned int *b,
                         unsigned int *c, unsigned int *d);
void get_cpu_name(char *cpu_name);
void get_cpu_info(char *cpu_name, char *architecture, unsigned int *family,
                  unsigned int *model, unsigned int *stepping);
#define LOG_LOCATION printf_com("%s:%d -->%s\n", __FILE__, __LINE__,__func__)
#define dbgprintf(a, ...) printf_com("%s():" a, __func__, ##__VA_ARGS__)
void printf_error(const char *format, ...);
void init_com1(void);
#endif
