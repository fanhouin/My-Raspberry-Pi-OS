#ifndef SCHED_H_
#define SCHED_H_
#include <stddef.h>
#include <list.h>

#define MAX_THREAD 0x10
#define STACK_SIZE 0x1000

enum thread_state{
    NOUSE,
    WAIT,
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
    unsigned long lr; // retrun address
    unsigned long sp; // stact pointer
}CpuContext;


typedef struct _Thread{
    struct list_head list;
    CpuContext ctx;
    enum thread_state state;
    int id;
    void *ustack_addr;
    void *kstack_addr;
    void *code_addr; // use in exec
    unsigned int code_size; // use in exec
}Thread;

extern Thread* get_current();
extern void cpu_switch_to(Thread* prev, Thread* next);

void init_thread_pool_and_head();
Thread *thread_create();
void kill_zombie();
void idle_thread();
void schedule();
void kernel_main();
void delay(unsigned int);
void foo();

void print_run_thread();
 

#endif