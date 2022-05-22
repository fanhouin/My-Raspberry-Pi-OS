#include <uart.h> 
#include <string.h>
#include <exc.h>
#include <syscall.h>
#include <user_syscall.h>

void exception_handler( unsigned long long esr, 
                        unsigned long long elr, 
                        unsigned long long spsr,
                        TrapFrame *trapFrame){

    /* 
     * EC, bits [31:26] -> Exception Class. Indicates the reason for the exception that this register holds information about.
     * ec = 0b010101: SVC instruction execution in AArch64 state.
     * https://developer.arm.com/documentation/ddi0601/2021-12/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-?lang=en
    */
    unsigned int ec = esr >> 26;
    if(ec == 0b010101){
        unsigned int syscall_id = trapFrame->x[8];
        syscall_handler(syscall_id, trapFrame);
    }
    else{
        uart_puts("---------Exception Handler---------\n[*] Exception type: Synchronous\n");
        print_string(UITOHEX, "[*] spsr_el1: 0x", spsr, 1);
        print_string(UITOHEX, "[*] elr_el1: 0x", elr, 1);
        print_string(UITOHEX, "[*] esr_el1: 0x", esr, 1);
    }

}


void syscall_handler(unsigned int syscall_id, TrapFrame *trapFrame){
    switch (syscall_id)
    {
    case GET_PID:
        sys_getpid(trapFrame);
        break;
    case UART_READ:
        sys_uart_read(trapFrame);
        break;
    case UART_WRITE:
        sys_uart_write(trapFrame);
        break;
    case EXEC:
        sys_exec(trapFrame);
        break;
    case FORK:
        sys_fork(trapFrame);
        break;
    case EXIT:
        sys_exit(trapFrame);
        break;
    case MBOX_CALL:
        sys_mbox_call(trapFrame);
        break;
    case KILL:
        sys_kill(trapFrame);
        break;
    case SIGNAL_:
        sys_signal_register(trapFrame);
        break;
    case SIGKILL:
        sys_signal_kill(trapFrame);
        break;
    case SIGRETURN:
        sys_sigreturn(trapFrame);
        break;

    default:
        break;
    }
}
