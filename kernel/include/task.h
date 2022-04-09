#ifndef	TASK_H_
#define TASK_H_
#define NULL 0x0

typedef void (*Handler)();

typedef struct _Task{
    Handler handler;
    unsigned int priority;
    struct _Task *next;
    struct _Task *prev;
}Task;

void add_task(Handler, unsigned int);
void do_task();

#endif