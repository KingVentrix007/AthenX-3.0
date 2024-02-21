#ifndef __FAT_FILELIB_H__
#define __FAT_FILELIB_H__

#include "fat_opts.h"
#include "fat_access.h"
#include "fat_list.h"
#include "types.h"
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#ifndef SEEK_CUR
    #define SEEK_CUR    1
#endif

#ifndef SEEK_END
    #define SEEK_END    2
#endif

#ifndef SEEK_SET
    #define SEEK_SET    0
#endif

#ifndef EOF
    #define EOF         (-1)
#endif

//-----------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------
struct sFL_FILE;

struct cluster_lookup
{
    uint32 ClusterIdx;
    uint32 CurrentCluster;
};

typedef struct sFL_FILE
{
    uint32                  parentcluster;
    uint32                  startcluster;
    uint32                  bytenum;
    uint32                  filelength;
    int                     filelength_changed;
    char                    path[FATFS_MAX_LONG_FILENAME];
    char                    filename[FATFS_MAX_LONG_FILENAME];
    uint8                   shortfilename[11];

#ifdef FAT_CLUSTER_CACHE_ENTRIES
    uint32                  cluster_cache_idx[FAT_CLUSTER_CACHE_ENTRIES];
    uint32                  cluster_cache_data[FAT_CLUSTER_CACHE_ENTRIES];
#endif

    // Cluster Lookup
    struct cluster_lookup   last_fat_lookup;

    // Read/Write sector buffer
    uint8                   file_data_sector[FAT_SECTOR_SIZE];
    uint32                  file_data_address;
    int                     file_data_dirty;

    // File fopen flags
    uint8                   flags;
#define FILE_READ           (1 << 0)
#define FILE_WRITE          (1 << 1)
#define FILE_APPEND         (1 << 2)
#define FILE_BINARY         (1 << 3)
#define FILE_ERASE          (1 << 4)
#define FILE_CREATE         (1 << 5)

    struct fat_node         list_node;
} FL_FILE;
typedef struct {
    char name[256]; // Adjust the size as needed
} Entry;
typedef struct stat
{
    __kernel_dev_t     st_dev;          /* ID of device containing file */
    __kernel_off_t     st_size;         /* total size, in bytes */
    __kernel_time_t    st_atime;        /* time of last access */
    __kernel_time_t    st_mtime;        /* time of last modification */
    __kernel_time_t    st_ctime;        /* time of last status change */
};

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
#define MAX 100
// External
void                fl_init(void);
void                fl_attach_locks(void (*lock)(void), void (*unlock)(void));
int                 fl_attach_media(fn_diskio_read rd, fn_diskio_write wr);
void                fl_shutdown(void);

int fatfs_add_special_entries(struct fatfs *fs, uint32 dirCluster, uint32 parentCluster);
// Standard API
void*               fl_fopen(const char *path, const char *modifiers);
void                fl_fclose(void *file);
int                 fl_fflush(void *file);
int                 fl_fgetc(void *file);
char *              fl_fgets(char *s, int n, void *f);
int                 fl_fputc(int c, void *file);
int                 fl_fputs(const char * str, void *file);
int                 fl_fwrite(const void * data, int size, int count, void *file );
int                 fl_fread(void * data, int size, int count, void *file );
int                 fl_fseek(void *file , long offset , int origin );
int                 fl_fgetpos(void *file , uint32 * position);
long                fl_ftell(void *f);
int                 fl_feof(void *f);
int                 fl_remove(const char * filename);
int                 fprintf(void *fp,const char *format,...);

// Equivelant dirent.h
typedef struct fs_dir_list_status    FL_DIR;
typedef struct fs_dir_ent            fl_dirent;

FL_DIR*             fl_opendir(const char* path, FL_DIR *dir);
int                 fl_readdir(FL_DIR *dirls, fl_dirent *entry);
int                 fl_closedir(FL_DIR* dir);

// Extensions
void fl_listdirectory(const char *path, Entry dirs[MAX], Entry files[MAX], int *dir_count, int *file_count);
int fl_output_disable();
int fl_output_enable();
int                 fl_createdirectory(const char *path);
int                 fl_is_dir(const char *path);

int                 fl_format(uint32 volume_sectors, const char *name);

//Custom Extensions:

// Test hooks
#ifdef FATFS_INC_TEST_HOOKS
struct fatfs*       fl_get_fs(void);
#endif

//-----------------------------------------------------------------------------
// Stdio file I/O names
//-----------------------------------------------------------------------------
// #define USE_FILELIB_STDIO_COMPAT_NAMES
#define USE_FILELIB_STDIO_COMPAT_NAMES
#ifdef USE_FILELIB_STDIO_COMPAT_NAMES

#define FILE            FL_FILE
#define fopen(a,b)      fl_fopen(a, b)
#define fclose(a)       fl_fclose(a)
#define fflush(a)       fl_fflush(a)
#define fgetc(a)        fl_fgetc(a)
#define fgets(a,b,c)    fl_fgets(a, b, c)
#define fputc(a,b)      fl_fputc(a, b)
#define fputs(a,b)      fl_fputs(a, b)
#define fwrite(a,b,c,d) fl_fwrite(a, b, c, d)
#define fread(a,b,c,d)  fl_fread(a, b, c, d)
#define fseek(a,b,c)    fl_fseek(a, b, c)
#define fgetpos(a,b)    fl_fgetpos(a, b)
#define ftell(a)        fl_ftell(a)
#define feof(a)         fl_feof(a)
#define remove(a)       fl_remove(a)
#define mkdir(a)        fl_createdirectory(a)
#define rmdir(a)        0
extern int stderr;
extern int stdout;
extern int stdin;
#endif
int ferror(FL_FILE *stream);
int ungetc(int character, FL_FILE *stream);
long get_file_size(FL_FILE *file);
int is_file(const char *path);
int rename(const char* old_name, const char* new_name);
#endif
