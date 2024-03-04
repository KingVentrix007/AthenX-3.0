#include "scheduler.h"
#include "string.h"
#include "printf.h"
#include "fat_filelib.h"
#include "command.h"
// #include "mem.h"
#include "exe.h"
#include "io_ports.h"
#include "stdlib.h"
#include "termianl.h"
void loop_test();
char current_path[FATFS_MAX_LONG_FILENAME];
char **executables_path;
// Function to parse command line arguments
char** parse_command(char* cmd_line, int* argc) {
    // Allocate memory for the arguments array
    LOG_LOCATION;
    char** argv = (char**)malloc(sizeof(char*) * 64);
    LOG_LOCATION;
    if(argv == NULL)
    {
        printf("allocation error\n");
        return NULL;
    }
    if (!argv) {
        *argc = -1;
        return NULL;
    }

    int arg_count = 0;
    char* token = strtok(cmd_line, " ");
    while (token != NULL) {
        // Check if the token starts with a double quote
        if (token[0] == '"') {
            // Find the closing double quote
            char* closing_quote = strchr(token + 1, '"');
            if (closing_quote == NULL) {
                // If there's no closing quote, treat the whole token as one argument
                argv[arg_count] = token;
                arg_count++;
            } else {
                // Allocate memory for the argument
                argv[arg_count] = (char*)kmalloc(sizeof(char) * (closing_quote - token + 1));
                if (!argv[arg_count]) {
                    // Memory allocation failed
                    *argc = -1;
                    return NULL;
                }
                // Copy the argument between the quotes
                strncpy(argv[arg_count], token + 1, closing_quote - token - 1);
                argv[arg_count][closing_quote - token - 1] = '\0';
                arg_count++;
            }
            // Move to the next token
            token = strtok(NULL, " ");
        } else {
            // Allocate memory for the argument
            argv[arg_count] = (char*)kmalloc(sizeof(char) * (strlen(token) + 1));
            if (!argv[arg_count]) {
                // Memory allocation failed
                *argc = -1;
                return NULL;
            }
            // Copy the argument
            strcpy(argv[arg_count], token);
            arg_count++;
            // Move to the next token
            token = strtok(NULL, " ");
        }
    }

    *argc = arg_count;
    return argv;
}

int cls()
{
    clear_screen();
    set_terminal_postion_x(0);
    set_terminal_postion_y(0);
}
// Function to free memory allocated by the command parser
void free_command(char** argv, int argc) {
    for (int i = 0; i < argc; i++) {
        LOG_LOCATION;
        free(argv[i]);
    }
    free(argv);
}

void set_cwd(char *path) {
    if (path == NULL) {
        // If path is NULL, return without changing the current directory
        return;
    }
    
    char current_path[FATFS_MAX_LONG_FILENAME];
    
    if (path[0] == '/') {
        // If the path starts with '/', it's an absolute path
        chdir(path);
    } else if (strcmp(path, "..") == 0) {
        // If the path is "..", move back one directory
        chdir("..");
    } else {
        // Otherwise, it's a relative path
        strcpy(current_path,getcwd()); // Get the current working directory
        strcat(current_path, "/"); // Append "/" to the current path
        strcat(current_path, path); // Append the provided path
        chdir(current_path);
    }
}

char *get_cwd() {
    static char current_path[FATFS_MAX_LONG_FILENAME];
    getcwd(current_path, FATFS_MAX_LONG_FILENAME);
    return current_path;
}
void ls()
{
    printf("Listing files\n");

    int num_dir;
    int num_files;
    Entry files[MAX];
    Entry dirs[MAX];
    printf_com("Calling fl_listdir()\n");
    fl_listdirectory("/",dirs,files,&num_dir,&num_files);
}
void date()
{
    
}
void shutdown()
{
    acpiPowerOff();
}
int cmd(char *command)
{

    int argc;
        LOG_LOCATION;

    char** argv = parse_command(command, &argc);
        LOG_LOCATION;
    
    if (argv == NULL) {
        printf("Error: Memory allocation failed.\n");
        LOG_LOCATION;

        return 1;
    }
    for (size_t i = 0; i < argc; i++)
    {
        printf_debug("\n%s: ", argv[i]);
    }
    
    LOG_LOCATION;

    if(strcmp(argv[0],"cd") ==0 )
    {
        set_cwd(argv[1]);
        LOG_LOCATION;

    }
    else if (strcmp(argv[0],"ls") ==0)
    {
        ls();
        LOG_LOCATION;
        
    }
    else if (strcmp(argv[0],"exe") == 0)
    {
        LOG_LOCATION;

        //  load_elf_file("/test/test.elf", argc, argv);
    }
    else if(strcmp(argv[0],"shutdown") == 0)
    {
        shutdown();
    }
    else if(strcmp(argv[0],"cls") == 0)
    {
        cls();
    }

    else
    {
        LOG_LOCATION;

        // int num_dir;
        // int num_files;
        // Entry files[MAX];
        // Entry dirs[MAX];
        
        // fl_output_disable();
      
        // fl_listdirectory("/", dirs, files, &num_dir, &num_files);
        // // fl_output_enable();
        char program_name[MAX];
        memset(program_name,0,sizeof(program_name));
        strcat(program_name,argv[0]);
        
        strcat(program_name,".elf");
        char path[100] = "/bin/";
        char tmp[100];
        memset(tmp,0,sizeof(tmp));
        LOG_LOCATION;

        strcat(tmp,path);
        strcat(tmp,program_name);
        FL_FILE *f = fl_fopen(tmp,"r");
        // printf("tmp = %s\n",tmp);
        LOG_LOCATION;

        if(f == NULL)
        {
            printf("Command %s not recognized\n",argv[0]);
        }
        else
        {
            fl_fclose(f);
            LOG_LOCATION;
            // int argc = arg_count+1;
            LOG_LOCATION;

            char argv_elf[100][100];
            for (size_t i = 0; i < argc; i++)
            {
                strcpy(argv_elf[i], argv[i]);
            }
            
            LOG_LOCATION;
            for (size_t i = 0; i < argc; i++)
            {
                // printf("%s\n", argv_elf[i]);
            }
            
            execute_file(tmp,argc,argv_elf);
            // printf_com(">tmp = %s\n", tmp);
            
            LOG_LOCATION;

            memset(tmp,0,sizeof(tmp));
            LOG_LOCATION;
            // return 0;
        }
        
        // printf("Program name: %s\n",program_name);
        // for (size_t i = 0; i < num_files; i++)
        // {
        //     printf("Program file %s\n",files[i].name);
        //     if(strcmp(files[i].name, program_name) == 0)
        //     {
        //         printf("Running program %s\n", program_name);
        //          free_command(argv, argc);
        //         break;
        //     }
        // }
        // printf("Command %s not recognized\n", argv[0]);
        // return -1;
        

    }
    printf_com("freeing memory\n");
    free_command(argv, argc);
    printf_com("made it here\n");
    return 0;
}
    

void loop_test()
{
    for (size_t i = 0; i < 10; i++)
    {
        printf("%d", i);

    }
    TerminateProcess();
    
}