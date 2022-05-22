#include <sched.h>
#include <list.h>
#include <malloc.h>
#include <string.h>
#include <stddef.h>
#include <allocator.h>
#include <irq.h>
#include <uart.h>
#include <syscall.h>

Thread *thread_pool;
Thread *run_thread_head;

void init_thread_pool_and_head(){
    thread_pool = (Thread*)kmalloc(sizeof(Thread) * MAX_THREAD);
    memset((char *)thread_pool, 0, sizeof(Thread) * MAX_THREAD);
    for(unsigned int i = 0; i < MAX_THREAD; i++){
        INIT_LIST_HEAD(&thread_pool[i].list);
        thread_pool[i].state = NOUSE;
        thread_pool[i].id = i;
        thread_pool[i].ustack_addr = NULL;
        thread_pool[i].kstack_addr = NULL;
        thread_pool[i].code_addr = NULL;
        thread_pool[i].code_size = 0;
    }

    run_thread_head = (Thread *)kmalloc(sizeof(Thread));
    memset((char *)run_thread_head, 0, sizeof(Thread));
    INIT_LIST_HEAD(&run_thread_head->list);
    run_thread_head->id = -1;
}

Thread *thread_create(void(*func)()){
    unsigned int idx;
    for(idx = 0; idx < MAX_THREAD; idx++){
        if(thread_pool[idx].state == NOUSE)
            break;
    }
    if(idx == MAX_THREAD) return NULL;

    Thread *new_thread = &thread_pool[idx];
    new_thread->state = WAIT;
    new_thread->ustack_addr = kmalloc(STACK_SIZE);
    new_thread->kstack_addr = kmalloc(STACK_SIZE);
    new_thread->ctx.fp = (unsigned long)new_thread->kstack_addr + STACK_SIZE;
    new_thread->ctx.sp = (unsigned long)new_thread->kstack_addr + STACK_SIZE;
    new_thread->ctx.lr = (unsigned long)func;


    print_string(UITOHEX, "new_thread->ustack: ", (unsigned long long )new_thread->ustack_addr, 0);
    uart_puts(" | ");
    print_string(UITOHEX, "new_thread->kstack: ", (unsigned long long )new_thread->kstack_addr, 1);


    list_add_tail(&new_thread->list, &run_thread_head->list);

    return new_thread;
}

void idle_thread(){
    while(1){
        // kill zombie
        kill_zombie();
        // call schedule
        schedule();
    }
}

void kill_zombie(){
    disable_irq();
    struct list_head *pos;
    list_for_each(pos, &run_thread_head->list){
        Thread *tmp = (Thread *)pos;
        if(tmp->state == EXIT){
            kfree(tmp->ustack_addr);
            kfree(tmp->kstack_addr);
            tmp->state = NOUSE;
            tmp->ustack_addr = NULL;
            tmp->kstack_addr = NULL;
            if(tmp->code_addr != NULL){
                kfree(tmp->code_addr);
            }
            tmp->code_addr = NULL;
            tmp->code_size = 0;
            list_del_entry(&tmp->list);
        }
    }
    enable_irq();
}

void schedule(){
    disable_irq();
    Thread *curr_thread = get_current();
    Thread *next_thread = curr_thread;
    do{
        next_thread = (Thread *)next_thread->list.next;
    }while(list_is_head(&next_thread->list, &run_thread_head->list) || next_thread->state == EXIT);

    // print_string(UITOHEX, "curr: ", curr_thread->id, 0);
    // uart_puts(" | ");
    // print_string(UITOHEX, "next: ", next_thread->id, 1);

    enable_irq();
    cpu_switch_to(curr_thread, next_thread);
}



void kernel_main() {
    disable_irq();
    /* the first thread that is fake thread */
    Thread curr_thread;

    thread_create(idle_thread);
    /* create the idle thread first */
    for(int i = 0; i < 5; i++){
        thread_create(foo);
    }

    Thread *next_thread = (Thread *)run_thread_head->list.next;

    print_run_thread();
    enable_irq();
    cpu_switch_to(&curr_thread, next_thread);
}

void delay(unsigned int time){
    unsigned long long system_timer = 0;
    unsigned long long frq = 0;
    asm volatile(
        "mrs %0, cntpct_el0\n\t"
        "mrs %1, cntfrq_el0\n\t"
        :"=r"(system_timer), "=r"(frq)
    );
    unsigned long long expired_time = system_timer + 1000;
    while(system_timer <= expired_time){
        asm volatile(
            "mrs %0, cntpct_el0\n\t"
            :"=r"(system_timer)
        );
    }
}

void foo(){
    for(int i = 0; i < 2; i++) {
        print_string(UITOA, "Thread id: ", get_current()->id, 0);
        print_string(UITOA, " ", i, 1);
        // printf("Thread id: %d %d\n", get_current()->task_id, i);
        delay(1);
        schedule();
    }
    do_exit();
}

void print_run_thread(){
    struct list_head *pos;
    unsigned int first = 1;
    list_for_each(pos, &run_thread_head->list){
        Thread *tmp = (Thread *)pos;
        if(first){
            print_string(UITOA, "run_thread: pid", tmp->id, 0);
            first = 0;
        } 
        else print_string(UITOA, " -> pid", tmp->id, 0);
    }
    uart_puts("\n");
}