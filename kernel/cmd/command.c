#include "scheduler.h"
#include "string.h"
#include "printf.h"
#include "fat_filelib.h"
#include "command.h"
// #include "mem.h"
#include "exe.h"
void loop_test();
char current_path[FATFS_MAX_LONG_FILENAME];
// Function to parse command line arguments
char** parse_command(char* cmd_line, int* argc) {
    // Allocate memory for the arguments array
    char** argv = (char**)kmalloc(sizeof(char*) * 64);
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

// Function to free memory allocated by the command parser
void free_command(char** argv, int argc) {
    for (int i = 0; i < argc; i++) {
        kfree(argv[i]);
    }
    kfree(argv);
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
    int num_dir;
    int num_files;
    Entry files[MAX];
    Entry dirs[MAX];
    fl_listdirectory(getcwd(),dirs,files,&num_dir,&num_files);
}
int cmd(char *command)
{
    int argc;
    char** argv = parse_command(command, &argc);
    if (argv == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    if(strcmp(argv[0],"cd") ==0)
    {
        set_cwd(argv[1]);
    }
    else if (strcmp(argv[0],"ls") ==0)
    {
        ls();
    }
    else if (strcmp(argv[0],"exe") == 0)
    {
        //  load_elf_file("/test/test.elf", argc, argv);
    }
    else
    {
        int num_dir;
        int num_files;
        Entry files[MAX];
        Entry dirs[MAX];
        
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
        strcat(tmp,path);
        strcat(tmp,program_name);
        FILE *f = fl_fopen(tmp,"r");
        printf("tmp = %s\n",tmp);

        if(f == NULL)
        {
            printf("Command %s not recognized\n",argv[0]);
        }
        else
        {
            fclose(f);
            int argc = 3;
            char **argv = {"program_name", "arg1", "arg2", NULL};
            execute_file(tmp,argc,argv);
            memset(tmp,0,sizeof(tmp));
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
        return -1;
        

    }

    free_command(argv, argc);

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