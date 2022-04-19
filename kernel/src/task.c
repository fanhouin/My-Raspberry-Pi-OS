#include <task.h>
#include <uart.h>
#include <string.h>
#include <malloc.h>
#include <irq.h>

unsigned int curr_poriority = 100;
Task *task_head = NULL;
void add_task(Handler handler, unsigned int priority){
    Task *task = (Task*)kmalloc(sizeof(Task));
    memset((char *)task, 0, sizeof(Task));
    task->handler = handler;
    task->priority = priority;
    task->next = NULL;
    task->prev = NULL;

    if(task_head == NULL){
        task_head = task;
        return;
    }
    
    Task *tmp = task_head;
    while(tmp->next != NULL && task->priority > tmp->priority){
        tmp = tmp->next;
    }

    if(task->priority <= tmp->priority){
        if(tmp == task_head){
            task_head = task;
            task_head->next = tmp;
            tmp->prev = task_head;
        }
        else{
            task->next = tmp;
            task->prev = tmp->prev;
            tmp->prev->next = task;
            tmp->prev = task;
        }
    }
    else{
        tmp->next = task;
        task->prev = tmp;
    }

}


void do_task(){
    // for(Task *tmp = task_head; tmp != NULL; tmp = tmp->next){
    //     uart_sputs("->");
    //     if(tmp->priority == 1) uart_sputs("time");
    //     else uart_sputs("uart");
    // }
    // uart_sputs("\n");

    // NO PREEMPTION
    // while(task_head != NULL){
    //     if(task_head->next != NULL){
    //         Handler handler = task_head->handler;
    //         task_head = task_head->next;
    //         task_head->prev = NULL;
    //         enable_irq();
    //         handler();
    //     } 
    //     else{
    //         Handler handler = task_head->handler;
    //         task_head = NULL;
    //         enable_irq();
    //         handler();
    //     }
    // }

    while(task_head != NULL){
        // curr_task is highest priority task, return it and finish the curr_task.
        if(curr_poriority <= task_head->priority){
            // uart_sputs("curr_task is highest priority task, return it.\n");
            return;
        } 
        
        /*
        ** task_head is the highest priority task (highest priority task is the first task in the list)   
        ** new(highest priority) -> prev(curr_poriority) -> low priority task...
        */
        unsigned int prev_poriority = curr_poriority; // save current poriority     
        curr_poriority = task_head->priority;
        Handler handler = task_head->handler;
        enable_irq();
        handler();
        disable_irq();

        // new_task should be the low priority task / me / null
        Task *tmp = task_head;
        task_head = task_head->next;
        kfree(tmp);
        tmp = NULL;
        /*
        ** if prev_poriority(curr_poriority now) is highest priority task
        ** it means the prev_task need to be continued to finish the task.
        ** prev(curr_poriority) -> low priority task...
        */
        curr_poriority = prev_poriority; 
    }
    curr_poriority = 100;
}
