#include "fat_access.h"
#include "fat_filelib.h"
#include "unistd.h"
int access(const char *path, int mode) {
    // Check if the file exists
    if (mode & F_OK) {
        // Do nothing
    }

    // Check if the file is readable
    if (mode & R_OK) {
        // Do nothing
    }

    // Check if the file is writable
    if (mode & W_OK) {
        // Do nothing
    }

    // Check if the file is executable
    if (mode & X_OK) {
        // Do nothing
    }

    // Always return success
    return 0;
}