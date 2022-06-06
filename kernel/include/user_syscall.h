#ifndef	USER_SYSCALL_H_
#define	USER_SYSCALL_H_

#define GET_PID 0
#define UART_READ 1
#define UART_WRITE 2
#define EXEC 3
#define FORK 4
#define EXIT 5
#define MBOX_CALL 6
#define KILL 7
#define SIGNAL_ 8
#define SIGKILL 9
#define SIGRETURN 10
#define OPEN 11
#define CLOSE 12
#define WRITE 13
#define READ 14
#define MKDIR 15
#define MOUNT 16
#define CHDIR 17
#define LSEEK64 18
#define IOCTL 19

#endif

#ifndef __ASSEMBLY__

extern int getpid();
extern unsigned int uartread(char buf[], unsigned int size);
extern unsigned int uartwrite(const char buf[], unsigned int size);
extern int exec(const char *name, char *const argv[]);
extern int fork();
extern void exit();
extern int mbox_call(unsigned char ch, unsigned int *mbox);
extern void kill(int pid);
extern void singal(int SIGNAL, void (*handler)());
extern void signal_kill(int pid, int SIGNAL);
extern int open(const char *pathname, int flags);
extern int close(int fd);
extern long write(int fd, const void *buf, unsigned long count);
extern long read(int fd, void *buf, unsigned long count);
extern int mkdir(const char *pathname, unsigned mode);
extern int mount(const char *src, const char *target, const char *filesystem, unsigned long flags, const void *data);
extern int chdir(const char *path);
extern long lseek64(int fd, long offset, int whence);
extern int ioctl(int fd, unsigned long request, ...);

#endif  