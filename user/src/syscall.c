#include <syscall.h>
#include <sched.h>
#include <read.h>
#include <uart.h>
#include <list.h>
#include <irq.h>
#include <cpio.h>
#include <malloc.h>
#include <string.h>

extern Thread *run_thread_head;

/* 
 * Return value is x0
 * https://developer.arm.com/documentation/102374/0101/Procedure-Call-Standard
 */


/* Get current process’s id. */
void sys_getpid(TrapFrame *trapFrame){
    int pid = do_getpid();
    trapFrame->x[0] = pid;
}
int do_getpid(){
    return get_current()->id;
}

/* Return the number of bytes read by reading size byte into the user-supplied buffer buf. */
void sys_uart_read(TrapFrame *trapFrame){
    char *buf = (char *)trapFrame->x[0];
    unsigned int size = trapFrame->x[1];
    int idx = readnbyte(buf, size);
    trapFrame->x[0] = idx;
}

/* Return the number of bytes written after writing size byte from the user-supplied buffer buf. */
void sys_uart_write(TrapFrame *trapFrame){
    const char *buf = (char *)trapFrame->x[0];
    unsigned int size = trapFrame->x[1];
    for(unsigned int i = 0; i < size; i++){
        uart_putc(buf[i]);
    }
    trapFrame->x[0] = size;
}
void sys_exec(TrapFrame *trapFrame){
    const char *name = (const char *)trapFrame->x[0];
    char **const argv = (char **const)trapFrame->x[1];
    int success = do_exec(trapFrame, name, argv);
    trapFrame->x[0] = success;
}
int do_exec(TrapFrame *trapFrame, const char *name, char *const argv[]){
    /* check if the file info exist */
    file_info fileInfo = cpio_find_file_info(name);
    if(fileInfo.filename == NULL) return -1;

    /* check if the file can create in new memory */
    void *thread_code_addr = load_program(&fileInfo); 
    if(thread_code_addr == NULL) return -1;
    
    /* current thread will change the pc to new code addr */
    Thread *curr_thread = get_current();
    curr_thread->code_addr = thread_code_addr;
    curr_thread->code_size = fileInfo.filename_size;

    trapFrame->elr_el1 = (unsigned long long)curr_thread->code_addr;
    trapFrame->sp_el0 = (unsigned long long)curr_thread->ustack_addr + STACK_SIZE;

    return 0;
}

int kernel_exec(char *name){
    /* check if the file info exist */
    file_info fileInfo = cpio_find_file_info(name);
    if(fileInfo.filename == NULL) return -1;

    /* check if the file can create in new memory */
    void *thread_code_addr = load_program(&fileInfo); 
    if(thread_code_addr == NULL) return -1;

    Thread *new_thread = thread_create(thread_code_addr);
    new_thread->code_addr = thread_code_addr;
    new_thread->code_size = fileInfo.filename_size;
    print_string(UITOHEX, "new_thread->code_addr: 0x", (unsigned long long)new_thread->code_addr, 1);

    set_period_timer_irq();
    asm volatile(
        "mov x0, 0x0\n\t"
        "msr spsr_el1, x0\n\t"
        "msr tpidr_el1, %0\n\t"
        "msr elr_el1, %1\n\t"
        "msr sp_el0, %2\n\t"
        "mov sp, %1\n\t"
        "eret\n\t"
        ::"r"(new_thread),
        "r"(new_thread->code_addr),
        "r"(new_thread->ustack_addr + STACK_SIZE),
        "r"(new_thread->kstack_addr + STACK_SIZE)
        : "x0"
    );
    return 0; 
}

void *load_program(file_info *fileInfo){
    void *thread_code_addr = kmalloc(fileInfo->datasize);
    if(thread_code_addr == NULL) return NULL;

    memcpy(thread_code_addr, fileInfo->data, fileInfo->datasize);

    return thread_code_addr;
}


void sys_fork(TrapFrame *trapFrame){
    do_fork();
}
void do_fork(){
    // Thread *new_thread;
    // TrapFrame *curr_tf, *new_tf;
    // void *start;
}

void sys_exit(TrapFrame *trapFrame){
    int status = trapFrame->x[0];
    do_exit(status);
}
/* Terminate the current process. */
void do_exit(int status){
    disable_irq();

    Thread *exit_thread = get_current();
    exit_thread->state = EXIT;
    
    enable_irq();
    schedule();
    // cpu_switch_to(exit_thread, next_thread);
}

void sys_mbox_call(TrapFrame *trapFrame){

}
void sys_kill(TrapFrame *trapFrame){

}