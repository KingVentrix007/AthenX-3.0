#include "fat_filelib.h"
#include "kheap.h"
#include "logging.h"
#include "stdbool.h"
#include "file.h"
char *_cwd;
size_t _cwd_len;
char _fixed_cwd[FATFS_MAX_LONG_FILENAME];
bool use_cwd;
int init_fs()
{
    _cwd = kmalloc(FATFS_MAX_LONG_FILENAME);
    if(_cwd == NULL)
    {
        logging(3,__LINE__,__func__,__FILE__,"FS: Failed to allocate memory for _cwd. Defaulting to fixed size buffer\n");
        return -1;
    }
    
}


int chdir(const char *path)
{
    if(use_cwd == true)
    {
        char *tmp = kmalloc(_cwd_len);
        strcat(tmp,"/");
        strcat(tmp,path);
        if(fl_is_dir(tmp) == 0)
        {
            strcpy(_cwd,tmp);
            _cwd_len = _cwd_len+1+strlen(path)+2;
            kfree(tmp);
            return 0;
        }
        kfree(tmp);
        return -1;
    }

}