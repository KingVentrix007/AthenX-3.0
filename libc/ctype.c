#include "ctype.h"

int isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int isalpha(char c) {
    return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')));
}
int tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}
int isdigit(int c) {
    return (c >= '0' && c <= '9');
}
int isxdigit(int c) {
    return (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}
int toupper(int c) {
    if (islower(c)) {
        // If the character is a lowercase letter, convert it to uppercase
        return c - ('a' - 'A');
    } else {
        // If the character is not a lowercase letter, return it unchanged
        return c;
    }
}
int islower(int c) {
    return (c >= 'a' && c <= 'z');
}