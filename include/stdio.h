#ifndef __STDIO__H
#define __STDIO__H
#include <stddef.h>
#include <stdarg.h>
//Defines
#define EOF (-1)
//Typedefs for stdio
typedef long fpos_t;

//Fuction declarations
char *getcwd();
int chdir(const char *path);
void *fopen(const char *path,const char *modifiers);
size_t fread(void *ptr, size_t size, size_t nmemb, void *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, void *stream);
int fputc(int c, void *stream);
int fputs(const char *str, void *stream);
int fclose(void *stream);
int fseek(void *stream, long offset, int whence);
int fgetpos(void *stream, fpos_t *pos);
long ftell(void *stream);
int feof(void *stream);
char *fgets(char *s, int n, void *f);
int fgetc(void *f);
int mkdir(const char *path);
int rmdir(const char *path);
int fprintf(void *fp,const char *format,...);
int getchar(void);
int ungetc(int c, void *stream);
//Streams
extern int stdin;
extern int stdout;
extern int stderr;
#endif