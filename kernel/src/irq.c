#include <irq.h>
#include <timer.h>
#include <task.h>
#include <uart.h>
#include <string.h>
#include <sched.h>
#include <signal.h>
#include <syscall.h>

extern Thread *run_thread_head;

void irq_handler(unsigned long long spsr, TrapFrame *trapFrame){     
    // uart_sputs("---------IRQ Handler---------\n");
    if(*CORE0_IRQ_SOURCE & 0x2) Time_interrupt(spsr);
    else if(*CORE0_IRQ_SOURCE & 0x100) GPU_interrupt();

    /* check the spsr if it is from user mode */
    spsr &= 0b1111;
    if(spsr == 0x0){
        check_sig_queue(trapFrame);
    } 
}

void Time_interrupt(unsigned long long spsr){
    // unsigned long long frq = 0;
    // asm volatile("mrs %0, cntfrq_el0\n\t" :"=r"(frq));
    // print_string(UITOHEX, "spsr = ", (unsigned long long)spsr, 1);
    // spsr &= 0b1111;
    // if(spsr == 0x0){
    //     // disable_timer_irq();
    //     // add_task(timer_interrupt_handler_el0, 1);
    //     // do_task();
    //     timer_interrupt_handler_el0();
    // }
    // else{
        // disable_timer_irq();
        // add_task(timer_interrupt_handler, 1);
        // do_task();
        timer_interrupt_handler();
        Thread *next = (Thread *)run_thread_head->list.next;
        if((Thread *)next->list.next != run_thread_head){
            //uart_puts("go to schedule\n");
            schedule();
        }
        
    // }
}


/* 
    AUX_MU_IIR:
    On read this register shows the interrupt ID bit Interrupt ID
    bit[2:1]
    00 : No interrupts
    01 : Transmit holding register empty
    10 : Receiver holds valid byte
    11 : <Not possible>

    AUX_MU_IER_REG:
    bit 1: Enable Transmit interrupt
    bit 0: Enable Receive interrupt
*/


void GPU_interrupt(){
    // uart_sputs("GPU interrupt\n");
    if(*AUX_MU_IIR & TRANSMIT_HOLDING){ // Transmit interrupt
        // disable_AUX_MU_IER_w();
        // add_task(tran_interrupt_handler, 3);
        // do_task();
        tran_interrupt_handler();
        enable_irq();
    }
    else if(*AUX_MU_IIR & RECEIVE_VALID){ // Receive interrupt
        // disable_AUX_MU_IER_r();
        // disable_AUX_MU_IER_w();
        // add_task(recv_interrupt_handler, 2);
        // do_task();
        recv_interrupt_handler();
        enable_irq();
    }
    else{
        uart_puts("[*] AUX_MU_IIR: No interrupts\n");
    }
}


void enable_timer_irq(){
    asm volatile(
        "msr cntp_ctl_el0, %0\n\t"
        ::"r"(1)
    );
    *CORE0_TIMER_IRQ_CTRL |= 2;
}

void disable_timer_irq(){
    asm volatile(
        "msr cntp_ctl_el0, %0\n\t"
        ::"r"(0)
    );
    *CORE0_TIMER_IRQ_CTRL &= ~2;
}

void reset_timer_irq(unsigned long long expired_time){
    asm volatile(
        "msr cntp_cval_el0, %0\n\t"
        ::"r"(expired_time)
    );
}

void set_long_timer_irq(){
    asm volatile(
        "mov x1, 0xfffffffffffffff\n\t" // set a large second
        "msr cntp_cval_el0, x1\n\t" 
        :::"x1"
    );
}

void set_period_timer_irq(unsigned long long peried){
    unsigned long long system_timer = 0;
    asm volatile("mrs %0, cntpct_el0\n\t" :"=r"(system_timer));
    system_timer += peried;
    asm volatile(
        "msr cntp_tval_el0, %0\n\t" 
        ::"r"(system_timer)
    );
}

void enable_irq(){
    asm volatile("msr DAIFClr, 0xf");
}

void disable_irq(){
    asm volatile("msr DAIFSet, 0xf");
}

