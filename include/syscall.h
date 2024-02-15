#ifndef __SYSCALL__H
#define __SYSCALL__H

#define SYS_LOG 1
#define SYS_PANIC 2
#define SYS_TCB_SET 3
#define SYS_FUTEX_TID 4
#define SYS_FUTEX_WAIT 5
#define SYS_FUTEX_WAKE 6
#define SYS_ANON_ALLOCATE 7
#define SYS_ANON_FREE 8
#define SYS_OPEN 9
#define SYS_READ 10
#define SYS_SEEK 11
#define SYS_CLOSE 12
#define SYS_STAT 13
#define SYS_VM_MAP 14
#define SYS_VM_UNMAP 15
#define SYS_VM_PROTECT 16
#define SYS_PUTS 17
#define SYS_GETC 18
int system_call_handler_c(int syscall_number, int param1,int param2);
extern void system_call_handler();
int syscall(int syscall_number, int param1, int param2);
#endif