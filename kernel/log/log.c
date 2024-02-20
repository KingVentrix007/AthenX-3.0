#include "stdarg.h"
#include "printf.h"
#include "io_ports.h"
#include "logging.h"
#include "stdbool.h"
#include "fat_filelib.h"
bool can_write_to_log_file = false;




/**
 * 
 * @brief prints line info and message to host terminal to assist in debugging
 * 
 * @param log_level 
 * @param line __LINE__
 * @param function __func__
 * @param path __PATH__
 * @param format 
 * @param ... 
 * @return int 
 */
int logging(int log_level,int line, char *function, char *path,const char* format, ...)
{
  va_list va;
  va_start(va, format);
  char buffer[10000];
  const int ret = vsnprintf_(buffer, sizeof(buffer), format, va);
  va_end(va);
  // log_level: function at path:line\n\t:buffer
  if(log_level == 0)
  {
    //INFO
    printf_com("%s: %s at %s:%d -\n\t%s\n",INFO,function,path,line,buffer);
  }
  else if (log_level == 1)
  {
    //WARNING
     printf_com("%s: %s at %s:%d -\n\t%s\n",WARNING,function,path,line,buffer);
  }
  else if (log_level == 2)
  {
    //ERROR
     printf_com("%s: %s at %s:%d -\n\t%s\n",ERROR,function,path,line,buffer);
  }
  else
  {
    //OTHER
    printf_com("%d: %s at %s:%d -\n\t%s\n",log_level,function,path,line,buffer);
  }
  // if(can_write_to_log_file == false) 
  // {
  //   if(fat_inited() == true)
  //   {
  //     can_write_to_log_file = true;
  //   }
    
  // }
  // else if (can_write_to_log_file == true) 
  // {

   
  // }
  printf_com("Writeing to file\n"); 
  FL_FILE *fp = fl_fopen("/log/log.txt","w");
  if(fp == NULL)
  {
    printf_com("We have a problem writing to file\n");
  }
  fprintf(fp,"%d: %s at %s:%d -\n\t%s\n",log_level,function,path,line,buffer);
  fl_fclose(fp);
}