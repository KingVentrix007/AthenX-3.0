#include "stdio.h"

int main(int argc, char **argv)
{
    printf("HELLO FROM ELF \n");
    int num;
    char str[100];
    
    printf("Enter an integer and a string: ");
    scanf("%d %s", &num, str);
    
    printf("Integer: %d\n", num);
    printf("String: %s\n", str);
    return 0;
    // printf("%s\n", argv[0]);
}