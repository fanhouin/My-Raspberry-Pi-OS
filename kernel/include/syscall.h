#ifndef	SYSCALL_H_
#define	SYSCALL_H_
#include <cpio.h>
#include <sched.h>

typedef struct _TrapFrame{
    unsigned long long x[31];
    unsigned long long sp_el0;
    unsigned long long elr_el1;
    unsigned long long spsr_el1;
}TrapFrame;


void sys_getpid(TrapFrame *);
void sys_uart_read(TrapFrame *);
void sys_uart_write(TrapFrame *);
void sys_exec(TrapFrame *);
void sys_fork(TrapFrame *);
void sys_exit(TrapFrame *);
void sys_mbox_call(TrapFrame *);
void sys_kill(TrapFrame *);
void sys_signal_register(TrapFrame *);
void sys_signal_kill(TrapFrame *);
void sys_sigreturn(TrapFrame *);

int do_getpid();
int do_exec(TrapFrame *trapFrame, const char *name, char *const argv[]);
void *load_program(file_info *fileInfo);
int do_fork(TrapFrame *trapFrame);
void do_exit(int status);
int do_kill(int pid);
int do_signal_register(int signal, SigHandler handler);
int do_signal_kill(int pid, int signal);

int kernel_exec(char *name);

extern void after_fork();

#endif 