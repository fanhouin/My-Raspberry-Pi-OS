#include <task.h>
#include <uart.h>
#include <string.h>
#include <malloc.h>
#include <irq.h>
#include <list.h>

unsigned int curr_poriority = 100;

Task *task_head;

void init_task_head(){
    task_head = (Task*)kmalloc(sizeof(Task));
    INIT_LIST_HEAD(&task_head->list);
}

void add_task(Handler handler, unsigned int priority){
    Task *task = (Task*)kmalloc(sizeof(Task));
    memset((char *)task, 0, sizeof(Task));
    INIT_LIST_HEAD(&task->list);
    task->handler = handler;
    task->priority = priority;

    if(list_empty(&task_head->list)){
        list_add_tail(&task->list, &task_head->list);
        return;
    }

    struct list_head *pos;
    list_for_each(pos, &task_head->list){
        Task *tmp = (Task *)pos;
        if(task->priority <= tmp->priority){
            list_add(&task->list, tmp->list.prev);
        }
    }
    if(list_is_head(pos, &task_head->list)){
        list_add_tail(&task->list, &task_head->list);
    }
}


void do_task(){
    // for(Task *tmp = task_head; tmp != NULL; tmp = tmp->next){
    //     uart_sputs("->");
    //     if(tmp->priority == 1) uart_sputs("time");
    //     else uart_sputs("uart");
    // }
    // uart_sputs("\n");

    while(!list_empty(&task_head->list)){
        Task *t_task = (Task *)task_head->list.next;
        // curr_task is highest priority task, return it and finish the curr_task.
        if(curr_poriority <= t_task->priority){
            // uart_sputs("curr_task is highest priority task, return it.\n");
            return;
        } 
        
        /*
        ** task_head is the highest priority task (highest priority task is the first task in the list)   
        ** new(highest priority) -> prev(curr_poriority) -> low priority task...
        */
        unsigned int prev_poriority = curr_poriority; // save current poriority     
        curr_poriority = t_task->priority;
        Handler handler = t_task->handler;
        // new_task should be the low priority task / me / null

        enable_irq();
        handler();
        disable_irq();

        list_del(&t_task->list);
        kfree(t_task);
        t_task = NULL;
        /*
        ** if prev_poriority(curr_poriority now) is highest priority task
        ** it means the prev_task need to be continued to finish the task.
        ** prev(curr_poriority) -> low priority task...
        */
        curr_poriority = prev_poriority; 
    }
    curr_poriority = 100;
}
