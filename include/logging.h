#ifndef __LOGGING__H
#define __LOGGING__H

#define WARNING "WARNING"
#define ERROR "ERROR"
#define INFO "INFO"
int logging(int log_level,int line, char *function, char *path,const char* format, ...);
//LOGGING_WRAPPER

// #define LOG(...) do {logging()}
#endif