#include "scheduler.h"
#include "string.h"
#include "printf.h"
#include "fat_filelib.h"
#include "command.h"
#include "exe.h"
#include "io_ports.h"
#include "stdlib.h"
#include "termianl.h"
#include "stdio.h"
#include "debug.h"
#include "kernel.h"
#include "stdbool.h"
#include "elf.h"
char current_path[FATFS_MAX_LONG_FILENAME];
extern fs_active;
// Function prototypes
void loop_test();
char** parse_command(char* cmd_line, int* argc);
void free_command(char** argv, int argc);
void set_cwd(char *path);
char *get_cwd();
void ls();
void date();
void draw_kmap();
void shutdown();
void key_map();
int cmd(char *command);
void help();

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
                argv[arg_count] = (char*)malloc(sizeof(char) * (closing_quote - token + 1));
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
char* move_back_one_folder(char* path) {
    // Check if the path is empty or null
    if (path == NULL || strlen(path) == 0) {
        return path;
    }

    int len = strlen(path);
    char* result = (char*)malloc((len + 1) * sizeof(char)); // Allocate memory for the result string
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    // Find the last occurrence of path separator
    char* last_separator = strrchr(path, '/');
    if (last_separator == NULL) {
        last_separator = strrchr(path, '\\');
    }

    // If no separator found, return the original path
    if (last_separator == NULL) {
        strcpy(result, path);
        return result;
    }

    // Copy characters before the last separator to the result string
    strncpy(result, path, last_separator - path);
    result[last_separator - path] = '\0'; // Add null terminator

    return result;
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
    // printf("Setting %s\n", path);
    char current_path[FATFS_MAX_LONG_FILENAME];
    memset(current_path, 0, sizeof(current_path));
    // printf("Current path = [%s]\n", current_path);
    if (path[0] == '/') {
        // printf("First\n");
        // If the path starts with '/', it's an absolute path
        chdir(path);
    } else if (strcmp(path, "..") == 0) {
        // If the path is "..", move back one directory
        // char *new_path = move_back_one_folder(getcwd());
        // printf("new_path == %s\n", new_path);
        chdir(path);

    } else {
        // Otherwise, it's a relative path
        strcpy(current_path,getcwd()); // Get the current working directory
        // printf("current_path[strlen(current_path)-1] == %s || %d",current_path[strlen(current_path)-1],strlen(current_path)-1);
        if(path[0] != '/' && current_path[strlen(current_path)-1] != '/')
        {
            strcat(current_path, "/"); // Append "/" to the current path
        }
        // printf("getcwd() returning %s\n", getcwd());
        // printf("current_path = %s\n", current_path);
        strcat(current_path, path); // Append the provided path
        chdir(current_path);
    }
}
void help() {
    printf("Available commands:\n");
    printf("cd <directory>       Change current directory\n");
    printf("ls <paramater>       List files in the current directory\n");
    printf("date                 Display current date\n");
    printf("mmap                 Display kernel memory map\n");
    printf("shutdown             Shutdown the system\n");
    printf("cls                  Clear the screen\n");
    printf("key                  Display key mapping\n");
    printf("help                 Display this help message\n");
    printf("touch <file name>    Create a new file\n");
    printf("cad <file name>      Open a super basic file editor\n");
    printf("<program_name>       Execute program\n");
    printf("setd <drive_number>  Sets the current active drive\n");
}
char *get_cwd() {
    if(fs_active != true)
    {
        char *none = "/";
        return none;
    }
    char *current_path;
    current_path = getcwd();
    // getcwd(current_path, FATFS_MAX_LONG_FILENAME);
    return current_path;
}
void ls()
{
    // printf("Listing files -?(%s)\n",get_cwd());

    int num_dir;
    int num_files;
    Entry files[MAX];
    Entry dirs[MAX];
    dbgprintf("Calling fl_listdir()\n");
    if(fs_active != true)
    {
        printf("Is disabled\n");
        return;
    }
    fl_listdirectory(get_cwd(),dirs,files,&num_dir,&num_files);
}
void date()
{
    
}
void draw_kmap()
{
    print_kernel_memory_map(&g_kmap);
}
void shutdown()
{
    fs_shutdown();
    kheap_shutdown();
    acpiPowerOff();
}
void key_map()
{
    printf("Key map\n");
    printf("-\ta-z-A-Z-1-0: Standard\n");
    printf("-\tf2: Open error screen\n");
    printf("-\tf1,f3-f12: Not implemented\n");
    printf("-\tPS-PB: Not implemented\n");
    printf("-\tUp Down: Scroll up down\n");
    printf("-\tLeft Right: Not implemented\n");




}
int print_iris()
{
     printf("\033[0;31m"); // Set color to red
        printf("I");
        
        // Print "r" in green
        printf("\033[0;32m"); // Set color to green
        printf("r");

        // Print "i" in yellow
        printf("\033[0;33m"); // Set color to yellow
        printf("i");

        // Print "s" in blue
        printf("\033[0;34m"); // Set color to blue
        printf("s");

        // Reset color
        printf("\033[0m");
}
int get_primary_dev_num(const char *path) {
    // Extract the device number from the path
    int dev_num = 0;
    if (sscanf_(path, "%d:", &dev_num) == 1) {
        return dev_num;
    }
    return -1; // Invalid device number
}
const char* get_path_without_dev(const char *path) {
    // Return the path without the NUM: part
    const char *slash_pos = strchr(path, '/');
    if (slash_pos) {
        return slash_pos;
    }
    return path; // No '/' found, return the original path
}
uint64_t rand_v = 12345;
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

     if (strcmp(argv[0], "cd") == 0) {
        if (argc >= 2) {
            const char *path = argv[1];
            // Check if the path starts with NUM: or 1:/
            if ((path[1] == ':' && path[2] == '/') || (path[1] == ':' && strchr(path, '/') != NULL)) {
                int dev_num = get_primary_dev_num(path);
                if (dev_num != -1) {
                    set_primary_dev(dev_num);
                }
                path = get_path_without_dev(path);
            }
            set_cwd(path);
        } else {
            printf("%s\n", getcwd());
        }
        // LOG_LOCATION();
    }
    else if (strcmp(argv[0],"list") == 0)
    {
        if(argc < 2)
        {
            printf("\nSupported arguments:\n");
            printf("\t list -s: List all secondary storage devices\n");
        }
        else if (strcmp(argv[1],"-s") == 0)
        {
            printf("\n");
            list_devices();
        }
        
    }
    else if (strcmp(argv[0],"rand") == 0)
    {
        srand(rand_v*time_since_last_keypress);
         rand_v = rand();
            dbgprintf("%u\n", rand_v);  // Generate random numbers

        
         
    
    }
    
    else if (strcmp(argv[0],"format") == 0)
    {
        if(argc < 2)
        {
            printf("This is a VERY destructive command\nIt will reformat the ENTIRE current disk\nRun with '-sure' to format the disk\n");
        }
        else if (strcmp(argv[1], "-sure") == 0)
        {

            printf("\nFormatting disk\n");
             uint32_t sectors;
            if(argc == 3)
            {
                sectors = atoi(argv[2]);
            }
            else
            {   
            sectors= get_current_drive_sector_count();
            printf("Formatting %u sectors\n", sectors);

            }
            char name = "disk";
            fl_format(sectors,name);
        }
        
    }
    
    else if(strcmp(argv[0], "help") == 0)
    {
        if(argc < 2)
        {
            help();

        }
        else
        {
            printf("Current version doesn't support specific help commands\n");
        }
    }
    else if(strcmp(argv[0], "touch") == 0)
    {
        if(argc == 2)
        
        {
            
             FILE *fp = fopen(argv[1],"a");
            if(fp!= NULL)
            {
                printf("File %s was created\n",argv[1]);
            }
            else
            {
                printf("File %s was not created\n",argv[1]);
            }
            fclose(fp);
        }else
        {
            printf("Please provide 1 argument\n");
        }
       fs_active = true;
    }
    else if(strcmp(argv[0], "mmap") == 0)
    {
        printf("\n");
        draw_kmap();
    }
    else if(strcmp(argv[0],"reboot") ==0 )
    {
        reboot();
    }
    else if (strcmp(argv[0],"ls") ==0)
    {
        if(argc < 2)
        {
            ls();
            LOG_LOCATION;
        }
        else if(strcmp(argv[1],"-p") == 0)
        {
            int count = 0;
            while(executables_path[count] != NULL)
            {
                printf("Exec %s\n",executables_path[count]);
                count++;
            }
        }
        
        
    }
    else if (strcmp(argv[0],"exe") == 0)
    {
        LOG_LOCATION;

        //  load_elf_file("/test/test.elf", argc, argv);
    }
    else if(strcmp(argv[0],"cad") ==  0)
    {
        if(argc != 2)
        {
            printf("\nUsage: cad <filename>\n");
        }
        else
        {
            printf("\n");
        edit_file(argv[1]);
        }
        
    }
    else if(strcmp(argv[0],"shutdown") == 0)
    {
        shutdown();
    }
    else if(strcmp(argv[0],"cls") == 0)
    {
        cls();
        draw_ui_layout(1024,768);
        // set_terminal_postion_x(command_start_x);
        // set_terminal_postion_y(command_start_y);
    }
    else if(strcmp(argv[0],"setd") == 0)
    {
        if(argc < 2)
        {
            printf("\nCurrent drive : %d\n",get_primary_dev());
        }
        else
        {
            int ret = set_primary_dev(atoi(argv[1]));
            if(ret == -1)
            {
                printf("Error: Failed to set primary device\n");
            }
            else
            {
                printf("\nPrimary device %d\n", atoi(argv[1]));
            }
        }
    }
    else if (strcmp(argv[0], "iris") == 0)
    {
        printf("Welcome to ");
        print_iris();
        printf("\n");
        print_iris();
        printf(" Is a simple bash like shell. Currently, the number of commands supported are the bare minimum for testing purposes, but that hopefully will change at some point\n");
        print_iris();
        printf(" Is named after the greek goddess iris. Who is the Goddess of rainbows and messages\n");
        printf("Type 'help' for a list of available");
    }
    
    else if(strcmp(argv[0],"key") == 0)
    {
        key_map();
    }
    else if (strcmp(argv[0],"mkdir") == 0)
    {
        printf("Dir output %d\n",mkdir(argv[1]));
    }
    
    else
    {
        if(fs_active != true)
        {
            free_command(argv, argc);
            printf("\nCommand '\033[31m%s\033[0m' is not a valid command, and the file system is inactive\n",argv[0]);
            return 1;
            // return none;
        } 
        LOG_LOCATION;
        printf("\n");

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
            FILE *f_to_print = fopen(argv[0], "r");
            if(f_to_print == NULL)
            {
                printf("\nCommand '\033[31m%s\033[0m' is not a valid command,executable or file\n",argv[0]);
                
            }
            else
            {
                char *buffer = malloc(get_file_size(f_to_print));
                fread(buffer,1,get_file_size(f_to_print),f_to_print);
                printf("%s\n",buffer);
                free(buffer);
                fclose(f_to_print);
            }
        }
        else

        {
            fl_fclose(f);
            LOG_LOCATION;
            // int argc = arg_count+1;
            LOG_LOCATION;

            char argv_elf[100][100];
             memset(argv_elf, 0, sizeof(argv_elf));
            for (size_t i = 0; i < argc; i++)
            {
                strcpy(argv_elf[i], argv[i]);
            }
            
            LOG_LOCATION;
            for (size_t i = 0; i < argc; i++)
            {
                printf("%s\n", argv_elf[i]);
            }
            
            execute_file(tmp,argc,argv_elf);
            // dbgprintf(">tmp = %s\n", tmp);
            
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
    dbgprintf("freeing memory\n");
    free_command(argv, argc);
    dbgprintf("made it here\n");
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