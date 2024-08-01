#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_FUNCTION_NAME 256
#define MAX_PARAMS 10

typedef struct {
    char type[MAX_LINE_LENGTH];
    char name[MAX_FUNCTION_NAME];
} ParameterInfo;
// Function to remove trailing whitespace from a string
void removeTrailingWhitespace(char *str) {
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

// Function to remove leading whitespace from a string
void removeLeadingWhitespace(char *str) {
    char *src = str;
    char *dst = str;

    // Skip leading whitespace
    while (isspace((unsigned char)*src)) {
        src++;
    }

    // Copy non-whitespace characters
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0'; // Null-terminate the result
}
// Helper function to trim leading and trailing whitespace
void trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) { // All spaces?
        return;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    *(end + 1) = 0;
    
}
int index_of_char(const char *str, char c) {
    const char *pos = strchr(str, c);
    if (pos == NULL) {
        return -1; // Character not found
    }
    return pos - str; // Calculate index
}
// Function to parse parameter strings
int parse_parameters(const char *parms, ParameterInfo *params) {
    int count = 0;
    char buffer[MAX_LINE_LENGTH];
    strcpy(buffer, parms);

    char *token = strtok(buffer, ",");
    while (token != NULL && count < MAX_PARAMS) {
        removeLeadingWhitespace(token);
        removeTrailingWhitespace(token);
        int end_of_type_index = index_of_char(token,' ');
        while(token[end_of_type_index] == ' ' || token[end_of_type_index] == '*')
        {
            end_of_type_index++;
        }
        // printf("Token = [%s] [%d]\n",token,end_of_type_index);
        char type[MAX_LINE_LENGTH] = "";
        for (size_t i = 0; i < end_of_type_index; i++)
        {
            type[i] = token[i];
        }
        // printf("type = [%s]\n",type);
        // printf("type + 1 == [%s]\n",token + end_of_type_index);
        removeLeadingWhitespace(type);
        removeTrailingWhitespace(type);
        strcpy(params[count].type, type);
        
        strcpy(params[count].name, token + end_of_type_index);
        count++;
        token = strtok(NULL, ",");
    }
    return count;
}

// Example usage
int main() {
    ParameterInfo params[MAX_PARAMS];
    const char *example_parms = "int a, char *msg, int arr[10], float *ptr, void func()";
    int param_count = parse_parameters(example_parms, params);

    for (int i = 0; i < param_count; ++i) {
        printf("Parameter %d: Type: '%s', Name: '%s'\n", i, params[i].type, params[i].name);
    }

    return 0;
}
