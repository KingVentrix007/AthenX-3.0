
# LIBC

## ANSI

- `<assert.h>` - Contains macro definitions for testing program assertions.
- `<ctype.h>` - Contains functions for testing and mapping characters.
- `<errno.h>` - Contains macros for reporting error conditions.
- `<float.h>` - Defines macro constants specifying the implementation-specific properties of the floating-point types.
- `<limits.h>` - Defines macro constants specifying the implementation-specific properties of integer types.
- `<locale.h>` - Contains localization functions.
- `<math.h>` - Contains mathematical functions.
- `<setjmp.h>` - Provides a mechanism for nonlocal jumps.
- `<signal.h>` - Defines signal-handling functions.
- `<stdarg.h>` - Provides functions for handling variable argument lists.
- `<stddef.h>` - Defines several macros.
- `<stdio.h>` - Contains input/output functions.
- `<stdlib.h>` - Contains general utilities functions.
- `<string.h>` - Contains string-handling functions.
- `<time.h>` - Contains date and time functions.

## POSIX
- `<aio.h>` - Asynchronous I/O operations.
- `<arpa/inet.h>` - Definitions for internet operations.
- `<dirent.h>` - Directory entries.
- `<fcntl.h>` - File control options.
- `<fnmatch.h>` - Filename matching types.
- `<glob.h>` - Pathname pattern matching.
- `<grp.h>` - Group file.
- `<ifaddrs.h>` - Interface addresses.
- `<net/if.h>` - Network interfaces.
- `<netdb.h>` - Network database operations.
- `<netinet/in.h>` - Internet address family.
- `<netinet/tcp.h>` - Transmission control protocol.
- `<pthread.h>` - POSIX threads.
- `<pwd.h>` - Password file.
- `<sched.h>` - Execution scheduling.
- `<semaphore.h>` - POSIX semaphores.
- `<signal.h>` - Signal handling.
- `<spawn.h>` - Spawn processes.
- `<sys/ipc.h>` - Interprocess communication (IPC).
- `<sys/mman.h>` - Memory management.
- `<sys/msg.h>` - Message queues.
- `<sys/resource.h>` - Resource limits.
- `<sys/select.h>` - Synchronous I/O multiplexing.
- `<sys/sem.h>` - Semaphore operations.
- `<sys/shm.h>` - Shared memory.
- `<sys/socket.h>` - Socket functions.
- `<sys/stat.h>` - File status.
- `<sys/time.h>` - Time types.
- `<sys/types.h>` - Various types.
- `<sys/uio.h>` - I/O vectors.
- `<sys/un.h>` - Unix domain sockets.
- `<sys/utsname.h>` - Operating system name.
- `<sys/wait.h>` - Process status.
- `<termios.h>` - Terminal control.
- `<unistd.h>` - Standard symbolic constants and types.
- `<utime.h>` - File access and modification times.
- `<utmp.h>` - User accounting database.
- `<utmpx.h>` - Extended user accounting database.
- `<wchar.h>` - Wide character handling.
- `<wctype.h>` - Wide character classification.


## MLIBC
- [ ] `sys_libc_log(const char *message)`
- [ ] `sys_libc_panic()`
- [ ] `sys_tcb_set(void *pointer)`
- [ ] `sys_futex_tid()`
- [ ] `sys_futex_wait(int *pointer, int expected, const struct timespec *time)`
- [ ] `sys_futex_wake(int *pointer)`
- [ ] `sys_anon_allocate(size_t size, void **pointer)`
- [ ] `sys_anon_free(void *pointer, size_t size)`
- [ ] `sys_open(const char *pathname, int flags, mode_t mode, int *fd)`
- [ ] `sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read)`
- [ ] `sys_seek(int fd, off_t offset, int whence, off_t *new_offset)`
- [ ] `sys_close(int fd)`
- [ ] `sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf)`
- [ ] `sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window)`
- [ ] `sys_vm_unmap(void *pointer, size_t size)`
- [ ] `sys_vm_protect(void *pointer, size_t size, int prot)`
