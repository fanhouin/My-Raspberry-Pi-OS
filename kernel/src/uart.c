#include <uart.h>
#include <gpio.h>
#include <irq.h>
#include <string.h>

char read_buf[MAX_SIZE];
char write_buf[MAX_SIZE];
static unsigned int read_set_idx = 0;
static unsigned int read_get_idx = 0;
static unsigned int write_set_idx = 0;
static unsigned int write_get_idx = 0;


void uart_init(){
  *AUX_ENABLE     |= 1;   // Enable mini UART.
  *AUX_MU_CNTL    = 0;    // Disable transmitter and receiver during configuration.
  *AUX_MU_IER     = 0;    // Disable interrupt because currently you don’t need interrupt.
  *AUX_MU_LCR     = 3;    // Set the data size to 8 bit.
  *AUX_MU_MCR     = 0;    // Don’t need auto flow control.
  *AUX_MU_BAUD    = 270;  // Set baud rate and characteristics (115200 8N1) and map to GPIO 
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

  *ENABLE_IRQS_1 = 1 << 29;       // enable UART1 IRQ

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
  // while(!(*AUX_MU_LSR & 0x01)) {asm volatile("nop");}
  /* read it and return */
  char r = (char)(*AUX_MU_IO);
  /* convert carrige return to newline */
  return r == '\r'?'\n':r;
}

void recv_interrupt_handler(){
  /* read buffer is full, discard the new char */
  if((read_set_idx + 1) % MAX_SIZE == read_get_idx){
    disable_AUX_MU_IER_r();
    return;
  } 

  read_buf[read_set_idx] = uart_getc();
  read_set_idx = (read_set_idx + 1) % MAX_SIZE; /* reset the index if it reaches the end */
  /* enable receive interrupt after set the new char */
  enable_AUX_MU_IER_r(); 
}

char async_uart_getc(){
  // enable_AUX_MU_IER_r();
  /* wait until something is in the read buffer (read_set_idx != read_get_idx) */
  while(read_get_idx == read_set_idx) {asm volatile("nop");}

  disable_irq();
  char r = read_buf[read_get_idx]; /* read the char that set in read buffer already*/
  read_get_idx = (read_get_idx + 1) % MAX_SIZE; /* reset the index if it reaches the end */
  enable_irq();
  
  return r;
}

/* Display a char */
void uart_putc(unsigned int c){
  /* 
  wait until we can send
  0x20/bit_6 is set if the transmit FIFO is empty and the
  transmitter is idle. (Finished shifting out the last bit).
  */
  while(!(*AUX_MU_LSR & 0x20)) {asm volatile("nop");}

  /* write the character to the buffer */
  *AUX_MU_IO = c;
}

void tran_interrupt_handler(){
  /* finished sending the last char */
  if(write_get_idx == write_set_idx){
    disable_AUX_MU_IER_w();
    return;
  } 

  char c = write_buf[write_get_idx];
  uart_putc(c);
  write_get_idx = (write_get_idx + 1) % MAX_SIZE; /* reset the index if it reaches the end */

  /* finished sending the last char */
  if(write_get_idx == write_set_idx){
    disable_AUX_MU_IER_w();
    return;
  } 

  /* enable transmit interrupt to expect print next char */
  enable_AUX_MU_IER_w();
}

void async_uart_putc(unsigned int c){
  /* buffer is full, wait the sending char */
  while((write_set_idx + 1) % MAX_SIZE == write_get_idx) {enable_AUX_MU_IER_w();}
  

  disable_irq();
  write_buf[write_set_idx] = (char)c;
  write_set_idx = (write_set_idx + 1) % MAX_SIZE; /* reset the index if it reaches the end */
  enable_irq();

  /* enable transmit interrupt after set the new char */
  enable_AUX_MU_IER_w();
}


/* Display a string */
void uart_puts(char *s) {
  while(*s) async_uart_putc(*s++);
  /* convert newline to carrige return + newline */
  if(*(--s)=='\n') async_uart_putc('\r');
  
}

void uart_sputs(char *s) {
  while(*s) uart_putc(*s++);
  /* convert newline to carrige return + newline */
  if(*(--s)=='\n') uart_putc('\r');
  
}

void uart_nbyte(char *s, unsigned int len) {
  while(len--) {
    /* convert newline to carrige return + newline */
    if(*s=='\n') uart_putc('\r');
    uart_putc(*s++);
  }
}


void enable_AUX_MU_IER_r(){
  *AUX_MU_IER |= 1;
}

void enable_AUX_MU_IER_w(){
  *AUX_MU_IER |= 2;
}

void disable_AUX_MU_IER_r(){
  *AUX_MU_IER &= ~1;
}

void disable_AUX_MU_IER_w(){
  *AUX_MU_IER &= ~2;
}