#ifndef __CMD_HANDLER__H
#define __CMD_HANDLER__H
#define MAX_CMD_LENGTH 100
#define MAX_CMD_DESCRIPTION_LENGTH 1000
#define MAX_CMD_ARG_LENGTH 100
#define MAX_ARG_LENGTH 150
typedef struct arg_t
{
    char arg[MAX_ARG_LENGTH];
    char *description; // MUST be dynamically allocated
}arg;
typedef struct cmd_t
{
    char command[MAX_CMD_LENGTH];
    int (*handler)(int argc, char *argv[MAX_CMD_ARG_LENGTH]);
    char *description; // MUST be dynamically allocated

    arg *supported_args; // MUST be dynamically allocated

    
}cmd;

#endif