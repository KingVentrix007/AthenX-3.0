#include "scheduler.h"
#include "string.h"
#include "printf.h"
void loop_test();
int cmd(char *command)
{
    if(strcmp(command, "task") == 0)
    {
        printf("Starting new task\n");
        CreateProcess(loop_test);

    }
}

void loop_test()
{
    for (size_t i = 0; i < 10; i++)
    {
        printf("%d", i);

    }
    TerminateProcess();
    
}