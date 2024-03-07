#include "stdio.h"
#include "types.h"
#include "stdlib.h"
#include "string.h"
#include "debug_term.h"
#include "printf.h"
#include "scanf.h"
char *current_user;

int init_security()
{
    current_user = malloc(1024);
    if(current_user == NULL)
    {
        char buffer[1024];
        snprintf(buffer, 1024, "%s:%d (%s)", __FILE__, __LINE__, __func__);
        kernel_panic(buffer,"failed to create kernel login buffer");
    }
    else
    {
        //ATM user is fixed
        strcpy(current_user, "dev");
    }
}

int login()
{
    printf("Please enter your password for %s:",current_user);
    char password[1024];
    
    disable_echo();
    // set_echo_char('*');
    scanf_("%s",password);
    enable_echo();
    printf("Password: %s\n", password);
}