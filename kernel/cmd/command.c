#include "scheduler.h"
#include "string.h"
#include "printf.h"
#include "fat_filelib.h"
#include "command.h"
void loop_test();
char current_path[FATFS_MAX_LONG_FILENAME];

void set_cwd(char *path)
{

}
char *get_cwd()
{

}
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