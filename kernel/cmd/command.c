#include "scheduler.h"
#include "string.h"
#include "printf.h"
#include "fat_filelib.h"
#include "command.h"
#include "mem.h"
void loop_test();
char current_path[FATFS_MAX_LONG_FILENAME];
// Function to parse command line arguments
char** parse_command(char* cmd_line, int* argc) {
    // Allocate memory for the arguments array
    char** argv = (char**)sys_allocate_memory(sizeof(char*) * 64);
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
                argv[arg_count] = (char*)sys_allocate_memory(sizeof(char) * (closing_quote - token + 1));
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
            argv[arg_count] = (char*)sys_allocate_memory(sizeof(char) * (strlen(token) + 1));
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
        sys_free_memory(argv[i]);
    }
    sys_free_memory(argv);
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
        getcwd(current_path, FATFS_MAX_LONG_FILENAME); // Get the current working directory
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