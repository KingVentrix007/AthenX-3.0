#include "stdio.h"

int main(int argc, char argv[100][100])
{
    // printf("echo\n");
    // printf("Because IDK what I am doing, and copyed WAY to much code from Chat-GPT\n");
    // printf("This dosnt work properly(AT ALL), so it is disabled ATM,\n");
    if(argc < 2)
    {
        printf("Usage: echo [OPTIONS] <string>\n");
        return -1;
    }
    printf("%s\n",argv[1]);
    return 0; 
    // printf("%s\n", argv[0]);
} 