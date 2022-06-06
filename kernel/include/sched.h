#ifndef SCHED_H_
#define SCHED_H_
#include <stddef.h>
#include <list.h>
#include <vfs.h>

#define MAX_THREAD 0x100
#define MAX_SIG_HANDLER 0x20
#define STACK_SIZE 0x1000



enum thread_state{
    NOUSE,
    RUNNING,
    EXIT
};

typedef struct _cpu_context{
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp; // frame pointer, stack base address
    unsigned long lr; // retrun address after context switch
    unsigned long sp; // stact pointer
}CpuContext;

typedef void (*SigHandler)();
typedef struct sig_list_{
    struct list_head list;
    unsigned int ready;
    SigHandler handler;
}SignalInfo;

typedef struct _Thread{
    struct list_head list;
    CpuContext ctx;
    enum thread_state state;
    int id;
    void *ustack_addr;
    void *kstack_addr;
    void *code_addr; // use in exec
    unsigned int code_size; // use in exec

    /* signal */
    SignalInfo sig_info_pool[MAX_SIG_HANDLER]; // all signal info
    SignalInfo sig_queue_head; // ready queue
    void *sig_stack_addr;
    void *old_tp;

    /* vfs */
    char dir[MAX_PATHNAME_LEN * 16];
    Dentry *dentry;
    File *fd_table[MAX_FD_NUM]; // max 16 fd
}Thread;

extern Thread* get_current();
extern void cpu_switch_to(Thread* prev, Thread* next);

void init_thread_pool_and_head();
Thread *thread_create();
void kill_zombie();
void idle_thread();
void schedule();
void kernel_main();
void delay(unsigned long long);
void foo();

void print_run_thread();
 

#endif