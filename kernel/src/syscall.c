#include <syscall.h>
#include <sched.h>
#include <read.h>
#include <uart.h>
#include <list.h>
#include <irq.h>
#include <cpio.h>
#include <malloc.h>
#include <string.h>
#include <timer.h>
#include <mailbox.h>
#include <sched.h>
#include <signal.h>
#include <vfs.h>
#include <tmpfs.h>

extern Thread *thread_pool;
extern Thread *run_thread_head;
extern File **global_fd_table;
extern char *global_dir;
extern Dentry *global_dentry;

/* 
 * Return value is x0
 * https://developer.arm.com/documentation/102374/0101/Procedure-Call-Standard
 */


/* Get current process’s id. */
void sys_getpid(TrapFrame *trapFrame){
    disable_irq();
    int pid = do_getpid();
    trapFrame->x[0] = pid;
    enable_irq();
}
int do_getpid(){
    return get_current()->id;
}

/* Return the number of bytes read by reading size byte into the user-supplied buffer buf. */
void sys_uart_read(TrapFrame *trapFrame){
    disable_irq();
    char *buf = (char *)trapFrame->x[0];
    unsigned int size = trapFrame->x[1];
    enable_irq();
    int idx = async_readnbyte(buf, size);
    disable_irq();
    trapFrame->x[0] = idx;
    enable_irq();
}

/* Return the number of bytes written after writing size byte from the user-supplied buffer buf. */
void sys_uart_write(TrapFrame *trapFrame){
    disable_irq();
    const char *buf = (char *)trapFrame->x[0];
    unsigned int size = trapFrame->x[1];
    unsigned int i;
    enable_irq();
    for(i = 0; i < size; i++){
        uart_putc(buf[i]);
    }
    disable_irq();
    trapFrame->x[0] = i;
    enable_irq();
}
void sys_exec(TrapFrame *trapFrame){
    disable_irq();
    const char *name = (const char *)trapFrame->x[0];
    char **const argv = (char **const)trapFrame->x[1];
    int success = do_exec(trapFrame, name, argv);
    disable_irq();
    trapFrame->x[0] = success;
    enable_irq();
}


int do_exec(TrapFrame *trapFrame, const char *name, char *const argv[]){
    disable_irq();
    /* check if the file info exist */
    // file_info fileInfo = cpio_find_file_info(name);
    // if(fileInfo.filename == NULL) return -1;

    /* check if the file can create in new memory */
    unsigned long file_size = 0;
    void *thread_code_addr = vfs_load_program(name, &file_size);
    if(thread_code_addr == NULL) return -1;
    
    /* current thread will change the pc to new code addr */
    Thread *curr_thread = get_current();
    curr_thread->code_addr = thread_code_addr;
    curr_thread->code_size = file_size;

    /* set current trapFrame elr_el1(begin of code) and sp_el0(begin of user stack)*/
    trapFrame->elr_el1 = (unsigned long)curr_thread->code_addr;
    trapFrame->sp_el0 = (unsigned long)curr_thread->ustack_addr + STACK_SIZE;


    /* maybe need to reset the signal 0.0? */
    for(unsigned int i = 0; i < MAX_SIG_HANDLER; i++){
        curr_thread->sig_info_pool[i].ready = 0;
        curr_thread->sig_info_pool[i].handler = sig_default_handler;
        INIT_LIST_HEAD(&curr_thread->sig_info_pool[i].list);
    }
    INIT_LIST_HEAD(&curr_thread->sig_queue_head.list);
    if(curr_thread->sig_stack_addr != NULL)
        kfree(curr_thread->sig_stack_addr);
    if(curr_thread->old_tp != NULL)
        kfree(curr_thread->old_tp);
    curr_thread->sig_stack_addr = NULL;
    curr_thread->old_tp = NULL;

    /* reset the vfs info, except stdin, stdout, stderr */
    for(int i = 3; i < MAX_FD_NUM; i++){
        if(curr_thread->fd_table[i] != NULL){
            vfs_close(curr_thread->fd_table[i]);
            curr_thread->fd_table[i] = NULL;
        }
    }

    enable_irq();
    return 0;
}

int kernel_exec(char *name){
    disable_irq();
    /* check if the file info exist */
    // file_info fileInfo = cpio_find_file_info(name);
    // if(fileInfo.filename == NULL) return -1;

    /* check if the file can create in new memory */
    unsigned long file_size;
    void *thread_code_addr = vfs_load_program(name, &file_size); 
    if(thread_code_addr == NULL) return -1;

    Thread *new_thread = thread_create(thread_code_addr);
    new_thread->code_addr = thread_code_addr;
    new_thread->code_size = file_size;
    print_string(UITOHEX, "[*] kernel_exec: new_thread->code_addr: 0x", (unsigned long long)new_thread->code_addr, 1);

    /* copy the golbal dir / dentry in the new_thread*/
    strcpy(new_thread->dir, global_dir);
    new_thread->dentry = global_dentry;
    /* copy fd table */
    for(int i = 0; i < MAX_FD_NUM; i++){
        new_thread->fd_table[i] = global_fd_table[i];
    }

    set_period_timer_irq();
    sched_timeout("omg");
    enable_irq();
    asm volatile(
        "mov x0, 0x0\n\t"
        "msr spsr_el1, x0\n\t"
        "msr tpidr_el1, %0\n\t"
        "msr elr_el1, %1\n\t"
        "msr sp_el0, %2\n\t"
        "mov sp, %3\n\t"
        "eret\n\t"
        ::"r"(new_thread),
        "r"(new_thread->code_addr),
        "r"(new_thread->ustack_addr + STACK_SIZE),
        "r"(new_thread->kstack_addr + STACK_SIZE)
        : "x0"
    );
    return 0; 
}

void *cpio_load_program(file_info *fileInfo){
    void *thread_code_addr = kmalloc(fileInfo->datasize);
    if(thread_code_addr == NULL) return NULL;
    memcpy(thread_code_addr, fileInfo->data, fileInfo->datasize);
    return thread_code_addr;
}

void *vfs_load_program(const char *pathname, unsigned long *size){
    File *file;
    int err = vfs_open(pathname, 0, &file);
    if(err < 0) return NULL;
    TmpfsInode *inode_head = (TmpfsInode *)file->vnode->internal;
    void *thread_code_addr = kmalloc(inode_head->size);
    if(thread_code_addr == NULL) return NULL;
    int read_size = vfs_read(file, thread_code_addr, inode_head->size);
    if(read_size < 0){
        kfree(thread_code_addr);
        thread_code_addr = NULL;
        uart_puts("[x] vfs_load_program: load program error\n");
        return NULL;
    }
    
    /* read the inode data */
    // size_t offset = 0;
    // char buf[MAX_SIZE];
    // int read_size;
    // while(1){
    //     read_size = vfs_read(file, buf, MAX_SIZE);
    //     if(read_size <= 0){
    //         break;
    //     } 
    //     memcpy(thread_code_addr + offset, buf, read_size);
    //     offset += read_size;
    // }
    // if(read_size < 0){
    //     kfree(thread_code_addr);
    //     thread_code_addr = NULL;
    //     uart_puts("[x] vfs_load_program: load program error\n");
    //     return NULL;
    // } 
    *size = inode_head->size;
    vfs_close(file);
    return thread_code_addr;
}


void sys_fork(TrapFrame *trapFrame){
    disable_irq();
    int child_pid = do_fork(trapFrame);
    disable_irq();
    trapFrame->x[0] = child_pid;
    enable_irq();
}

int do_fork(TrapFrame *trapFrame){
    disable_irq();
    Thread *curr_thread = get_current();

    // void *thread_code_addr = kmalloc(curr_thread->code_size);
    // if(thread_code_addr == NULL) return -1;

    Thread *new_thread = thread_create(curr_thread->code_addr);
    new_thread->code_addr = curr_thread->code_addr;
    new_thread->code_size = curr_thread->code_size;


    /* copy the code */
    // memcpy((char *)new_thread->code_addr, (char *)curr_thread->code_addr, new_thread->code_size);

    /* copy user stack */
    memcpy((char *)new_thread->ustack_addr, (char *)curr_thread->ustack_addr, STACK_SIZE);
    /* copy trap frame (kernel stack) */
    TrapFrame *new_trapFrame = (TrapFrame *)((char *)new_thread->kstack_addr + STACK_SIZE - sizeof(TrapFrame));
    memcpy((char *)new_trapFrame, (char *)trapFrame, sizeof(TrapFrame));
    /* copy context */
    memcpy((char *)&new_thread->ctx, (char *)&curr_thread->ctx, sizeof(CpuContext));

    /* copy signal */
    memcpy((char *)&new_thread->sig_info_pool, (char *)&curr_thread->sig_info_pool, MAX_SIG_HANDLER * sizeof(SignalInfo));
    for(unsigned int i = 0; i < MAX_SIG_HANDLER; i++){
        if(new_thread->sig_info_pool[i].ready > 0){
            list_add_tail(&new_thread->sig_info_pool[i].list, &new_thread->sig_queue_head.list);
        }
    }

    /* copy the golbal dir / dentry in the new_thread*/
    strcpy(new_thread->dir, global_dir);
    new_thread->dentry = global_dentry;


    /* copy fd table */
    for(int i = 0; i < MAX_FD_NUM; i++){
        File *tmp = global_fd_table[i];
        if(tmp != NULL){
            File *new_file = kmalloc(sizeof(File));
            new_file->f_ops = tmp->f_ops;
            new_file->f_pos = tmp->f_pos;
            new_file->vnode = tmp->vnode;
            new_file->flags = tmp->flags;
            new_thread->fd_table[i] = new_file;
        }
    }


    // print_string(UITOHEX, "(child)new_thread->code_addr: 0x", (unsigned long long)new_thread->code_addr, 1);

    
    /* return pid = 0 (child) */
    new_trapFrame->x[0] = 0;
    /* set new code return to after eret */
    // new_trapFrame->elr_el1 = (unsigned long)new_thread->code_addr + 
    //                         (trapFrame->elr_el1 - (unsigned long)curr_thread->code_addr);
    new_trapFrame->elr_el1 = trapFrame->elr_el1;

    /* set new pc return to after eret */
    new_trapFrame->sp_el0 = ((unsigned long)new_thread->ustack_addr + STACK_SIZE) -
                            (((unsigned long)curr_thread->ustack_addr + STACK_SIZE) - trapFrame->sp_el0);
   

    /* after context switch, child proc will load all reg from kernel stack, and return to el0 */
    new_thread->ctx.lr = (unsigned long)after_fork;
    new_thread->ctx.sp = (unsigned long)new_trapFrame;
    enable_irq();
    return new_thread->id;
}

void sys_exit(TrapFrame *trapFrame){
    disable_irq();
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
}

void sys_mbox_call(TrapFrame *trapFrame){
    disable_irq();
    unsigned char ch = trapFrame->x[0];
    unsigned int *mbox = (unsigned int *)trapFrame->x[1];
    int status = mailbox_call(mbox, ch);
    trapFrame->x[0] = status;
    enable_irq();
}


void sys_kill(TrapFrame *trapFrame){
    disable_irq();
    int pid = trapFrame->x[0];
    int status = do_kill(pid);
    if(status == -1) 
        print_string(UITOA, "[x] kill fail, pid: ", pid, 1);
    else
        print_string(UITOA, "[*] kill success, pid: ", pid, 1);

    enable_irq();
}

int do_kill(int pid){
    if(!(pid >= 0 && pid < MAX_THREAD))
        return -1;
    if(thread_pool[pid].state != RUNNING)
        return -1;

    thread_pool[pid].state = EXIT;
    enable_irq();
    schedule();
    return 0;
}

void sys_signal_register(TrapFrame *trapFrame){
    disable_irq();
    int signal = trapFrame->x[0];
    SigHandler handler = (SigHandler)trapFrame->x[1];
    int status = do_signal_register(signal, handler);
    if(status == -1) 
        print_string(UITOA, "[x] signal register fail, signal: ", signal, 1);
    // else
        // print_string(UITOA, "[*] signal register success, signal: ", signal, 1);
    enable_irq();
}

int do_signal_register(int signal, SigHandler handler){
    Thread *curr_thread = get_current();
    if(!(signal >= 0 && signal < MAX_SIG_HANDLER))
        return -1;
    
    curr_thread->sig_info_pool[signal].handler = handler;
    return 0;
}

void sys_signal_kill(TrapFrame *trapFrame){
    disable_irq();
    int pid = trapFrame->x[0];
    int signal = trapFrame->x[1];
    int status = do_signal_kill(pid, signal);

    if(status == -1) 
        print_string(UITOA, "[x] signal_kill fail, pid: ", pid, 1);
    // else
    //     print_string(UITOA, "[*] signal_kill ready, pid: ", pid, 1);

    enable_irq();
}

int do_signal_kill(int pid, int signal){
    if(!(pid >= 0 && pid < MAX_THREAD))
        return -1;
    if(thread_pool[pid].state != RUNNING)
        return -1;

    /* 
     * add the signal to the thread's ready queue 
     * if ther signal isn't in ready queue, add it.
     */
    if(thread_pool[pid].sig_info_pool[signal].ready == 0){
        list_add_tail(&thread_pool[pid].sig_info_pool[signal].list, &thread_pool[pid].sig_queue_head.list);
    }
    thread_pool[pid].sig_info_pool[signal].ready++;
    return 0;    
}

void sys_sigreturn(TrapFrame *trapFrame){
    disable_irq();
    Thread *current = get_current();
    /* load the old trap frame */
    memcpy((char *)trapFrame, (char *)current->old_tp, sizeof(TrapFrame));
    kfree(current->sig_stack_addr);
    kfree(current->old_tp);
    current->sig_stack_addr = NULL;
    current->old_tp = NULL;

    enable_irq();
}

void sys_open(TrapFrame *trapFrame){
    disable_irq();
    char *path = (char *)trapFrame->x[0];
    int flags = trapFrame->x[1];

    File *file = NULL;
    int status = vfs_open(path, flags, &file);
    unsigned int fd_idx = 0;
    if(status == 0){
        for(;fd_idx < MAX_FD_NUM; fd_idx++){
            if(global_fd_table[fd_idx] == NULL){
                global_fd_table[fd_idx] = file;
                trapFrame->x[0] = fd_idx;
                return;
            }
        }
        kfree(file);
        file = NULL;
        trapFrame->x[0] = -1;
    }
    else 
        trapFrame->x[0] = status;
    enable_irq();
}

void sys_close(TrapFrame *trapFrame){
    disable_irq();
    int fd = trapFrame->x[0];

    if(fd < 0 || fd >= MAX_FD_NUM)
        trapFrame->x[0] = -1;
    else if(global_fd_table[fd] == NULL)
        trapFrame->x[0] = -1;
    else{
        int status = vfs_close(global_fd_table[fd]);
        if(status == 0) global_fd_table[fd] = NULL;
        trapFrame->x[0] = status;
    }
    enable_irq();
}

void sys_write(TrapFrame *trapFrame){
    disable_irq();
    int fd = trapFrame->x[0];
    char *buf = (char *)trapFrame->x[1];
    int count = trapFrame->x[2];
    int status;

    if(fd < 0 || fd >= MAX_FD_NUM){
        trapFrame->x[0] = -1;
        goto DONE;
    }   
        
    if(global_fd_table[fd] == NULL){
        trapFrame->x[0] = -1;
        goto DONE;
    }   

    /* FIFO: uart file */
    // if(fd == 1 || fd == 2){
    //     /* stdout, write the data in uart file */
    //     status = vfs_write(global_fd_table[fd], buf, count);
    //     if(status < 0){
    //         trapFrame->x[0] = status;
    //         goto DONE;
    //     }
    //     /* reset the pos */
    //     vfs_lseek64(global_fd_table[fd], 0, SEEK_SET);

    //     /* stdin, read the data to the terminal */
    //     size_t thesize = 0;
    //     char buf2[MAX_SIZE];
    //     int read_size;
    //     while(1){
    //         memset(buf2, 0, MAX_SIZE);
    //         read_size = vfs_read(global_fd_table[fd], buf2, MAX_SIZE - 1);
    //         if(read_size <= 0){
    //             break;
    //         } 
    //         thesize += read_size;
    //         uart_puts(buf2);
    //     }
    //     vfs_lseek64(global_fd_table[fd], 0, SEEK_SET);
    //     trapFrame->x[0] = thesize;
    //     goto DONE;
    // }

    /* normal file */
    status = vfs_write(global_fd_table[fd], buf, count);
    trapFrame->x[0] = status;

DONE:
    enable_irq();
}

void sys_read(TrapFrame *trapFrame){
    disable_irq();
    int fd = trapFrame->x[0];
    char *buf = (char *)trapFrame->x[1];
    int count = trapFrame->x[2];

    if(fd < 0 || fd >= MAX_FD_NUM)
        trapFrame->x[0] = -1;
    else if(global_fd_table[fd] == NULL)
        trapFrame->x[0] = -1;
    else{
        /* stdin */
        // if(fd == 0){
        //     enable_irq();
        //     int idx = async_readnbyte(buf, count);
        //     disable_irq();
        //     int status = vfs_write(global_fd_table[0], buf, idx);
        //     vfs_lseek64(global_fd_table[0], 0, SEEK_SET);
        //     trapFrame->x[0] = status;
        //     goto DONE;
            
        //     // trapFrame->x[0] = idx;
        //     // goto DONE;
        // }
        int status = vfs_read(global_fd_table[fd], buf, count);
        trapFrame->x[0] = status;
    }
    
    enable_irq();
}

void sys_mkdir(TrapFrame *trapFrame){
    disable_irq();
    char *path = (char *)trapFrame->x[0];
    // int mode = trapFrame->x[1];
    int status = vfs_mkdir(path);
    trapFrame->x[0] = status;
    enable_irq();
}

void sys_mount(TrapFrame *trapFrame){
    disable_irq();
    char *path = (char *)trapFrame->x[1];
    char *fsname = (char *)trapFrame->x[2];
    int status = vfs_mount(path, fsname);
    trapFrame->x[0] = status;
    enable_irq();
}

void sys_chdir(TrapFrame *trapFrame){
    disable_irq();
    char *path = (char *)trapFrame->x[0];
    int status = vfs_chdir(path);
    trapFrame->x[0] = status;
    enable_irq();
}

void sys_lseek64(TrapFrame *trapFrame){
    disable_irq();
    int fd = trapFrame->x[0];
    int offset = trapFrame->x[1];
    int whence = trapFrame->x[2];
    // uart_puts("lseek64\n");
    int status = vfs_lseek64(global_fd_table[fd], offset, whence);
    trapFrame->x[0] = status;
    enable_irq();
}

void sys_ioctl(TrapFrame *trapFrame){
    disable_irq();

    enable_irq();
}