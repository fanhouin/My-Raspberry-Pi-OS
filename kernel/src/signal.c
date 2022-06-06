#include <signal.h>
#include <sched.h>
#include <syscall.h>
#include <irq.h>
#include <malloc.h>
#include <uart.h>

extern Thread *thread_pool;
extern Thread *run_thread_head;

void check_sig_queue(TrapFrame *trapFrame){
    disable_irq();
    Thread *current = get_current();


    // if(current->running_signal == 1)
    //     goto ENABLE_IRQ;
    if(list_empty(&current->sig_queue_head.list)){
        goto ENABLE_IRQ;
    }
    
    SignalInfo *sigInfo = (SignalInfo *)current->sig_queue_head.list.next;
    if(sigInfo->ready > 0){
        sigInfo->ready = 0;
        /* call the default handler(do_exit(0)) */
        if(sigInfo->handler == sig_default_handler){
            sigInfo->handler(); 
        }
        /* if not default handler, call the user signal handler */
        else{
            current->sig_stack_addr = kmalloc(STACK_SIZE);
            current->old_tp = kmalloc(sizeof(TrapFrame));
            /* save the trapFrame into old_ctx */
            memcpy((char*)current->old_tp, (char*)trapFrame, sizeof(TrapFrame));
            trapFrame->x[0] = (unsigned long)sigInfo->handler;
            trapFrame->elr_el1 = (unsigned long)sig_register_handler;
            trapFrame->sp_el0 = (unsigned long)current->sig_stack_addr + STACK_SIZE;
        }
        list_del(&sigInfo->list);
    }
ENABLE_IRQ:
    enable_irq();
}


void sig_register_handler(SigHandler handler){
    handler();
    asm volatile(
        "mov x8, 20\n\t"
        "svc 0\n\t"
    );
}


void sig_default_handler(){
    do_exit(0);
}