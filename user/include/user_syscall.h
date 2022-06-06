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

#endif  