#include "errno.h"
#include "stdio.h"
int errno;


void perror(const char *string)
{
    fputs(string,stderr);
}

const char* strerror(int errno_val) {
    switch(errno_val) {
        case 1:
            return "Operation not permitted";
        case 2:
            return "No such file or directory";
        case 3:
            return "No such process";
        case 4:
            return "Interrupted system call";
        case 5:
            return "I/O error";
        case 6:
            return "No such device or address";
        case 7:
            return "Argument list too long";
        case 8:
            return "Exec format error";
        case 9:
            return "Bad file number";
        case 10:
            return "No child processes";
        case 11:
            return "Try again";
        case 12:
            return "Out of memory";
        case 13:
            return "Permission denied";
        case 14:
            return "Bad address";
        case 15:
            return "Block device required";
        case 16:
            return "Device or resource busy";
        case 17:
            return "File exists";
        case 18:
            return "Cross-device link";
        case 19:
            return "No such device";
        case 20:
            return "Not a directory";
        case 21:
            return "Is a directory";
        case 22:
            return "Invalid argument";
        case 23:
            return "File table overflow";
        case 24:
            return "Too many open files";
        case 25:
            return "Not a typewriter";
        case 26:
            return "Text file busy";
        case 27:
            return "File too large";
        case 28:
            return "No space left on device";
        case 29:
            return "Illegal seek";
        case 30:
            return "Read-only file system";
        case 31:
            return "Too many links";
        case 32:
            return "Broken pipe";
        case 33:
            return "Math argument out of domain of func";
        case 34:
            return "Math result not representable";
        default:
            return "Unknown error";
    }
}