#include "ctypes.h"
int isalpha(int ch) {
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}
char upper(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        // If the character is a lowercase letter, convert to uppercase
        return ch - ('a' - 'A');
    } else {
        // If the character is not a lowercase letter, return as is
        return ch;
    }
}