#include "string.h"
#include "stdint.h"
#include "stddef.h"
void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;
    unsigned char v = (unsigned char)value;
    for (size_t i = 0; i < num; ++i) {
        p[i] = v;
    }
    return ptr;
}