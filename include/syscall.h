#ifndef __SYSCALL__H
#define __SYSCALL__H

int system_call_handler_c(int syscall_number, void *param);
extern void system_call_handler();
int syscall(int syscall_number, void * param1, int param2);
#endif