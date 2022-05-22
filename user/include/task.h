#ifndef	TASK_H_
#define TASK_H_
#include <stddef.h>
#include <list.h>
typedef void (*Handler)();





typedef struct _Task{
    struct list_head list;
    unsigned int priority;
    Handler handler;
}Task;


void init_task_head();
void add_task(Handler, unsigned int);
void do_task();

#endif