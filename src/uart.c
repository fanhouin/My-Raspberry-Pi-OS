#include <uart.h>
#include <gpio.h>

void uart_init(){
  *AUX_ENABLE     |= 1;   // Enable mini UART.
  *AUX_MU_CNTL    = 0;    // Disable transmitter and receiver during configuration.
  *AUX_MU_IER     = 0;    // Disable interrupt because currently you don’t need interrupt.
  *AUX_MU_LCR     = 3;    // Set the data size to 8 bit.
  *AUX_MU_MCR     = 0;    // Don’t need auto flow control.
  *AUX_MU_BAUD    = 270;  // Set baud rate to 115200
  *AUX_MU_IIR     = 6;    // No FIFO

  /* map UART1 to GPIO pins */
  register unsigned int reg;
  reg = *GPFSEL1;
  reg &= ~((7<<12) | (7<<15));    // clear gpio14,15 alternate function, check uart.h
  reg |= ((2<<12) | (2<<15));     // alternate function 5 (TXD1 and RXD1)
  *GPFSEL1 = reg;
  /* Enable pins 14 and 15 (disable pull-up/down, "floating" input pin with no pull-up or pull-down resistors) */
  *GPPUD = 0;                     
  /* Wait 150 cycles – this provides the required set-up time for the control signal */
  for(reg = 0; reg > 150; reg--) {asm volatile("nop");}
  /* Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to modify */
  *GPPUDCLK0 = (1<<14) | (1<<15);
  /* Wait 150 cycles – this provides the required set-up time for the control signal */
  for(reg = 0; reg > 150; reg--) {asm volatile("nop");}
  *GPPUDCLK0 = 0;                 // flush GPIO setup
  *AUX_MU_CNTL = 3;               // enable Tx, Rx
}

/* Set baud rate and characteristics (115200 8N1) and map to GPIO */
void uart_send(unsigned int c){
  /* 
  wait until we can send
  0x20/bit_6 is set if the transmit FIFO is empty and the
  transmitter is idle. (Finished shifting out the last bit).
  */
  while(!(*AUX_MU_LSR & 0x20)) {asm volatile("nop");}

  /* write the character to the buffer */
  *AUX_MU_IO = c;
}

/* Receive a character */
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
  while(!(*AUX_MU_LSR & 0x01)) {asm volatile("nop");}
  /* read it and return */
  char r = (char)(*AUX_MU_IO);
  /* convert carrige return to newline */
  return r == '\r'?'\n':r;
}

/* Display a string */
void uart_puts(char *s) {
  while(*s) {
      /* convert newline to carrige return + newline */
      if(*s=='\n')
          uart_send('\r');
      uart_send(*s++);
  }
}
