#include "io_ports.h"
#include "string.h"
#include "printf.h"
#include "debug_term.h"
#define COM1_PORT 0x3F8 // COM1 base port address
int com_enabled = 0; // 0 = false, 1 = true
/**
 * read a byte from given port number
 */
uint8 inportb(uint16 port) {
    uint8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * write a given byte to given port number
 */
void outportb(uint16 port, uint8 val) {
    asm volatile("outb %1, %0" :: "dN"(port), "a"(val));
}

/**
 * read 2 bytes(short) from given port number
 */
uint16 inports(uint16 port) {
    uint16 rv;
    asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

/**
 * write given 2 bytes(short) to given port number
 */
void outports(uint16 port, uint16 data) {
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}

/**
 * read 4 bytes(long) from given port number
 */
uint32 inportl(uint16 port) {
    uint32 rv;
    asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (port));
    return rv;
}

/**
 * write given 4 bytes(long) to given port number
 */
void outportl(uint16 port, uint32 data) {
    asm volatile ("outl %%eax, %%dx" : : "dN" (port), "a" (data));
}
void write_to_com1(uint8 data) {
    
    outportb(COM1_PORT, data);
}
void write_to_com1_string(char *s)
{
    // write_to_com1(':');
    if(com_enabled == 1)
    {
         for (size_t i = 0; i < strlen(s); i++)
        {
            write_to_com1(s[i]);
        }
    } 
   
    
}
void printf_error(const char *format, ...)
{
    // Define a buffer to hold the formatted string
    char buffer[256]; // Adjust the buffer size as needed

    // Initialize the variable argument list
    va_list args;
    va_start(args, format);

    // Format the string using vsnprintf
    vsnprintf(buffer, sizeof(buffer), format, args);

    // Cleanup the variable argument list
    va_end(args);

    // Call write_to_com1_string to write the formatted string to COM1
    char *msg = "ERROR: ";
    // write_to_com1_string(msg);
    // write_to_com1_string(buffer);
}
// void printf_debug(const char *format, ...)
// {
//      // Define a buffer to hold the formatted string
//     char buffer[256]; // Adjust the buffer size as needed

//     // Initialize the variable argument list
//     va_list args;
//     va_start(args, format);

//     // Format the string using vsnprintf
//     vsnprintf(buffer, sizeof(buffer), format, args);

//     // Cleanup the variable argument list
//     va_end(args);

//     // Call write_to_com1_string to write the formatted string to COM1
//     char *msg = "DEBUG: ";
//     write_to_com1_string(msg);
//     write_to_com1_string(buffer);
// }
void printf_com(const char* format, ...)
{
    // Define a buffer to hold the formatted string
    char buffer[256]; // Adjust the buffer size as needed

    // Initialize the variable argument list
    va_list args;
    va_start(args, format);

    // Format the string using vsnprintf
    vsnprintf(buffer, sizeof(buffer), format, args);

    // Cleanup the variable argument list
    va_end(args);

    // Call write_to_com1_string to write the formatted string to COM1
    // printf_debug("%s", buffer);
    char *msg = "DEBUG: ";
        write_to_com1_string(msg);
        write_to_com1_string(buffer);
}
void configure_com1(uint16 baud_rate, uint8 data_bits, uint8 stop_bits, uint8 parity) {
    // Disable interrupts
    outportb(COM1_PORT + 1, 0x00);
    
    // Set baud rate divisor
    uint16 divisor = 115200 / baud_rate;
    outportb(COM1_PORT + 3, 0x80);    // Enable DLAB (divisor latch access bit)
    outportb(COM1_PORT + 0, divisor & 0xFF); // Set low byte of divisor
    outportb(COM1_PORT + 1, divisor >> 8);   // Set high byte of divisor
    outportb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outportb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outportb(COM1_PORT + 4, 0x0B);    // Enable IRQs, RTS/DSR set
}

/**
 * Initialize COM1 serial port with default settings
 */
void init_com1() {
    configure_com1(9600, 8, 1, 0); // Default settings: 9600 baud, 8 data bits, 1 stop bit, no parity
    com_enabled = 1;
}

void cpuid(unsigned int code, unsigned int *a, unsigned int *b,
                         unsigned int *c, unsigned int *d) {
    asm volatile("cpuid"
                 : "=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d)
                 : "a" (code));
}
void get_cpu_name(char *cpu_name) {
    unsigned int eax, ebx, ecx, edx;
    
    // Execute CPUID instruction to get processor brand string
    cpuid(0x80000002, &eax, &ebx, &ecx, &edx);
    // Copy the brand string from each register to the cpu_name buffer
    *(unsigned int*)(cpu_name) = eax;
    *(unsigned int*)(cpu_name + 4) = ebx;
    *(unsigned int*)(cpu_name + 8) = ecx;
    *(unsigned int*)(cpu_name + 12) = edx;
    
    cpuid(0x80000003, &eax, &ebx, &ecx, &edx);
    *(unsigned int*)(cpu_name + 16) = eax;
    *(unsigned int*)(cpu_name + 20) = ebx;
    *(unsigned int*)(cpu_name + 24) = ecx;
    *(unsigned int*)(cpu_name + 28) = edx;
    
    cpuid(0x80000004, &eax, &ebx, &ecx, &edx);
    *(unsigned int*)(cpu_name + 32) = eax;
    *(unsigned int*)(cpu_name + 36) = ebx;
    *(unsigned int*)(cpu_name + 40) = ecx;
    *(unsigned int*)(cpu_name + 44) = edx;

    // Ensure null termination of the string
    cpu_name[16] = '\0';
}
void get_cpu_info(char *cpu_name, char *architecture, unsigned int *family,
                  unsigned int *model, unsigned int *stepping) {
    unsigned int eax, ebx, ecx, edx;
    
    // Execute CPUID instruction to get processor brand string
    cpuid(0x80000002, &eax, &ebx, &ecx, &edx);
    // Copy the brand string from each register to the cpu_name buffer
    *(unsigned int*)(cpu_name) = eax;
    *(unsigned int*)(cpu_name + 4) = ebx;
    *(unsigned int*)(cpu_name + 8) = ecx;
    *(unsigned int*)(cpu_name + 12) = edx;
    
    cpuid(0x80000003, &eax, &ebx, &ecx, &edx);
    *(unsigned int*)(cpu_name + 16) = eax;
    *(unsigned int*)(cpu_name + 20) = ebx;
    *(unsigned int*)(cpu_name + 24) = ecx;
    *(unsigned int*)(cpu_name + 28) = edx;
    
    cpuid(0x80000004, &eax, &ebx, &ecx, &edx);
    *(unsigned int*)(cpu_name + 32) = eax;
    *(unsigned int*)(cpu_name + 36) = ebx;
    *(unsigned int*)(cpu_name + 40) = ecx;
    *(unsigned int*)(cpu_name + 44) = edx;

    // Ensure null termination of the string
    cpu_name[48] = '\0';

    // Execute CPUID instruction to get architecture information
    cpuid(0, &eax, &ebx, &ecx, &edx);

    // Check vendor-specific signature to determine architecture
    char signature[13];
    memcpy(signature, &ebx, 4);
    memcpy(signature + 4, &edx, 4);
    memcpy(signature + 8, &ecx, 4);
    signature[12] = '\0';
    
    if (strcmp(signature, "GenuineIntel") == 0) {
        strcpy(architecture, "x86");
    } else if (strcmp(signature, "AuthenticAMD") == 0) {
        strcpy(architecture, "x86");
    } else {
        strcpy(architecture, "Unknown");
    }
    
    // Extract family, model, and stepping from eax
    *stepping = eax & 0x0F;
    *model = (eax >> 4) & 0x0F;
    *family = (eax >> 8) & 0x0F;
}