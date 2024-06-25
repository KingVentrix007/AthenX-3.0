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

int edit_file(char *file_path) {
    char path[FATFS_MAX_LONG_FILENAME];
    strcpy(path,file_path);
    FILE *fp = fopen(path, "r+");
    if (fp == NULL) {
        printf("Cant open file %s",path);
        return -1;
    }

    uint32_t file_size = get_file_size(fp);
    char *file_data = malloc(file_size + 100); // Extra space for edits
    if (file_data == NULL) {
        fclose(fp);
        return -1;
    }

    size_t bytesRead = fread(file_data, 1, file_size, fp);
    // fl_fclose(fp)
    if (bytesRead != file_size) {
        perror("Error reading file");
        free(file_data);
        fclose(fp);
        return 1;
    }
    file_data[file_size] = '\0';

    // Print the buffer (file contents)
    printf(">>%s", file_data);

    // Set cursor position to the end of the file content
    int current_cur_pos = file_size;

    char user_char;
    int num_chars = 0;
    while ((user_char = getch_()) != '\n' && user_char != EOF) {
        if (user_char == '\b') { // Backspace
            if (current_cur_pos > 0) {
                // printf("BACK\n");
                current_cur_pos--;
                num_chars--;
                file_data[current_cur_pos] = '\0';
                // printf("\b \b"); // Move cursor back, print space, move cursor back again
            }
        } else {
            file_data[current_cur_pos] = user_char;
            current_cur_pos++;
            num_chars++;
            // printf("%c", user_char); // Echo the character
        }
    }

    // Null-terminate the modified content
    if(current_cur_pos <file_size )
    {
        for (size_t i = current_cur_pos; i < file_size-current_cur_pos; i++)
        {
         file_data[i] = '\0';
        }
    }
    
    
    // file_data[current_cur_pos] = '\0';

    printf("\n%s",file_data);
    printf("\n%s\n",path);
    fclose(fp);
    FILE *write_file = fl_fopen(path, "w");
    int size;
    if(current_cur_pos > file_size)
    {
        size = current_cur_pos;
    }
    else
    {
        size = file_size;
    }
    for (size_t j = 0; j < size; j++)
    {
        fputc((int)file_data[j], write_file);
        // printf("%c",file_data[j]);
    }
    
    // size_t bytes_written = fl_fwrite(file_data,1,current_cur_pos,write_file);
    fclose(write_file);

    // Write the modified content back to the file
    return 0;
}