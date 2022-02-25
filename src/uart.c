#include "uart.h"
#include "gpio.h"

void uart_init(){
    *AUX_ENABLE     |= 1;   // Enable mini UART.
    *AUX_MU_CNTL    = 0;    // Disable transmitter and receiver during configuration.
    *AUX_MU_IER     = 0;    // Disable interrupt because currently you don’t need interrupt.
    *AUX_MU_LCR     = 3;    // Set the data size to 8 bit.
    *AUX_MU_MCR     = 0;    // Don’t need auto flow control.
    *AUX_MU_BAUD    = 270;  // Set baud rate to 115200
    *AUX_MU_IIR     = 6;    // No FIFO

    register unsigned int reg;

    /* map UART1 to GPIO pins */
    reg = *GPFSEL1;
    reg &= ~((7<<12) | (7<<15));    // gpio14,15 check uart.h
    reg |= ((7<<12) | (7<<15));     // alt5
    *GPFSEL1 = reg;
    /* Enable pins 14 and 15(disable pull-up/down, "floating" input pin with no pull-up or pull-down resistors) */
    *GPPUD = 0;                     
    /* Wait 150 cycles – this provides the required set-up time for the control signal */
    reg = 150; 
    while(reg--) { asm volatile("nop"); }
    /* Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to modify */
    *GPPUDCLK0 = (1<<14) | (1<<15);
    /* Wait 150 cycles – this provides the required set-up time for the control signal */
    reg = 150; 
    while(reg--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;                 // flush GPIO setup
    *AUX_MU_CNTL = 3;               // enable Tx, Rx
}

/*
Set baud rate and characteristics (115200 8N1) and map to GPIO
*/
void uart_send(unsigned int c){
    /* 
    wait until we can send
    0x20/bit_6 is set if the transmit FIFO is empty and the
    transmitter is idle. (Finished shifting out the last bit).
     */
    do{asm volatile("nop");} while(!(*AUX_MU_LSR & 0x20));

    /* write the character to the buffer */
    *AUX_MU_IO = c;
}

/*
Receive a character
*/
char uart_getc(){
    /* 
    wait until something is in the buffer 
    0x01/bit_1 is set if there was a receiver overrun. That is:
    one or more characters arrived whilst the receive
    FIFO was full. The newly arrived charters have been
    discarded. This bit is cleared each time this register is
    read. To do a non-destructive read of this overrun bit
    use the Mini Uart Extra Status register. 
    */
    do{asm volatile("nop");}while(!(*AUX_MU_LSR & 0x01));
    /* read it and return */
    char r;
    r = (char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r == '\r'?'\n':r;
}

/*
Display a string
*/
void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}
