#ifndef TYPES_H
#define TYPES_H

#define NULL 0

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;
typedef unsigned long long uint64;
typedef uint8 byte;
typedef uint16 word;
typedef uint32 dword;
typedef unsigned int __kernel_dev_t;   // dev_t
typedef unsigned long __kernel_ino_t;  // ino_t
typedef unsigned int __kernel_mode_t;  // mode_t
typedef unsigned short __kernel_nlink_t; // nlink_t
typedef unsigned int __kernel_uid_t;   // uid_t
typedef unsigned int __kernel_gid_t;   // gid_t
typedef long long __kernel_off_t;      // off_t
typedef long __kernel_time_t;          // time_t


typedef enum {
    FALSE,
    TRUE
} BOOL;

#endif

