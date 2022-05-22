#include <timer.h>
#include <uart.h>
#include <string.h>
#include <malloc.h>
#include <irq.h>

int printAfter2Second = 0;
Timer *head = NULL;
void add_timer(TimerTask task, unsigned long long expired_time, void *args){
    unsigned long long system_timer = 0;
    unsigned long long frq = 0;

    asm volatile(
        "mrs %0, cntpct_el0\n\t"
        "mrs %1, cntfrq_el0\n\t" 
        :"=r"(system_timer), "=r"(frq)
    );

    Timer *timer = (Timer*)kmalloc(sizeof(Timer));
    memset((char *)timer, 0, sizeof(Timer));
    timer->expired_time = system_timer + expired_time * frq;
    timer->task = task;
    timer->args = args;
    timer->next = NULL;
    timer->prev = NULL;

    if(head == NULL){
        head = timer;
        reset_timer_irq(head->expired_time);
        return;
    }
    Timer *tmp = head;
    while(tmp->next != NULL && timer->expired_time > tmp->expired_time){
        tmp = tmp->next;
    }

    if(timer->expired_time <= tmp->expired_time){
        if(tmp == head){
            head = timer;
            head->next = tmp;
            tmp->prev = head;
            reset_timer_irq(head->expired_time);
        }
        else{
            timer->next = tmp;
            timer->prev = tmp->prev;
            tmp->prev->next = timer;
            tmp->prev = timer;
        }
    }
    else{
        tmp->next = timer;
        timer->prev = tmp;
    }

}


void timer_interrupt_handler(){
    // while(1){
    //     uart_puts("abcdefghijklmnopqrstuvwxyz\n");
    // }
    while(head != NULL){
        if(head->next != NULL){
            Timer *tmp = head;
            head->task(head->args);
            head = head->next;
            head->prev = NULL;
            kfree(tmp);
            tmp = NULL;
            unsigned long long system_timer = 0;
            asm volatile("mrs %0, cntpct_el0\n\t" :"=r"(system_timer));
            if(head->expired_time > system_timer){
                reset_timer_irq(head->expired_time);
                break;
            }
        } 
        else{
            head->task(head->args);
            kfree(head);
            head = NULL;
        }
    }
    if(head == NULL) set_long_timer_irq();
    if(printAfter2Second == 0) {
        add_timer(timeout_print, 2, "[*] After Two Second, Hello User\n# ");
        printAfter2Second = 1;
    }
    enable_timer_irq();
}

void timer_interrupt_handler_el0(){
    head = NULL;
    uart_puts("Time interrupt\n");
    set_period_timer_irq();
    enable_timer_irq();
}

void timeout_print(void *args){
    uart_puts((char*)args);
}