#include "stdio.h"

int main(int argc, char argv[100][100])
{
    // printf("echo\n");
    if(argc < 2)
    {
        printf("Usage: echo [OPTIONS] <string>\n");
        return -1;
    }
    printf(">%s\n",argv[0]);
    return 0; 
    // printf("%s\n", argv[0]);
} 