#ifndef UART_H_
#define UART_H_
#include <gpio.h>

#define AUX_BASE        (MMIO_BASE + 0x00215000) //MMIO base address + AUX offset
#define AUX_ENABLE      ((volatile unsigned int*)(AUX_BASE+0x4))
#define AUX_MU_IO       ((volatile unsigned int*)(AUX_BASE+0x40))
#define AUX_MU_IER      ((volatile unsigned int*)(AUX_BASE+0x44))
#define AUX_MU_IIR      ((volatile unsigned int*)(AUX_BASE+0x48))
#define AUX_MU_LCR      ((volatile unsigned int*)(AUX_BASE+0x4C))
#define AUX_MU_MCR      ((volatile unsigned int*)(AUX_BASE+0x50))
#define AUX_MU_LSR      ((volatile unsigned int*)(AUX_BASE+0x54))
#define AUX_MU_MSR      ((volatile unsigned int*)(AUX_BASE+0x58))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(AUX_BASE+0x5C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(AUX_BASE+0x60))
#define AUX_MU_STAT     ((volatile unsigned int*)(AUX_BASE+0x64))
#define AUX_MU_BAUD     ((volatile unsigned int*)(AUX_BASE+0x68))
#define ENABLE_IRQS_1   ((volatile unsigned int*)(MMIO_BASE+0x0000B210))


/* Display a char */
void uart_init();

/* Send a character */
void uart_putc(unsigned int);

/* Receive a character */
char uart_getc();
/* Recv a new char in read buffer */
void recv_interrupt_handler();
/* Async receive a character */
char async_uart_getc();

/* Display a string */
void uart_puts(char*);

void uart_sputs(char*);
/* Transmit a character */
void tran_interrupt_handler();
/* Async send a character */
void async_uart_putc(unsigned int);

void uart_nbyte(char *, unsigned int);

void enable_AUX_MU_IER_r();
void enable_AUX_MU_IER_w();
void disable_AUX_MU_IER_r();
void disable_AUX_MU_IER_w();

#endif