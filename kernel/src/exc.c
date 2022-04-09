#include <uart.h> 
#include <string.h>
#include <exc.h>

void exception_handler(unsigned long long what, 
                    unsigned long long esr, 
                    unsigned long long elr, 
                    unsigned long long spsr){

    char buf[10];
    uart_puts("---------Exception Handler---------\n");
    uart_puts("[*] Exception type: ");
    uart_puts("Synchronous");

    uart_puts("\n");
    uart_puts("[*] spsr_el1: 0x");
    uitohex(buf, spsr);
    uart_puts(buf);
    uart_puts("\n");

    uart_puts("[*] elr_el1: 0x");
    uitohex(buf, elr);
    uart_puts(buf);
    uart_puts("\n");

    uart_puts("[*] esr_el1: 0x");
    uitohex(buf, esr);
    uart_puts(buf);
    uart_puts("\n");

}