#include "stdio.h"
#include "syscall.h"
#include "stdlib.h"
#include <stdarg.h>
void my_printf(const char *format, const char *str) {
    while (*format) {
        if (*format == '%' && *(format + 1) == 's') {
            // Replace %s with the string provided
            while (*str) {
                syscall(SYS_PUTS, *str, 1); // Print each character of the string
                str++;
            }
            format += 2; // Skip past "%s"
        } else {
            // Just print the current character
            syscall(SYS_PUTS, *format, 1);
            format++;
        }
    }
}


int main(int argc, char argv[100][100])
{
    // // printf("echo\n");
    // // printf("Because IDK what I am doing, and copyed WAY to much code from Chat-GPT\n");
    // // printf("This dosnt work properly(AT ALL), so it is disabled ATM,\n");
    if(argc < 2)
    {
        my_printf("Hello, %s!\n", "World");
        return 0;
    }
    // char msg[100] = "HELLO";
    // strcpy(msg,argv[1]);
    my_printf("%s",argv[1]);
    // if(argc < 2)
    // {
    //     printf("Usage: echo [OPTIONS] <string>\n");
    //     return -1;
    // }
    // printf("%s\n",argv[1]);
    return 0; 
    // printf("%s\n", argv[0]);
} 