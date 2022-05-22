#include <read.h>
#include <uart.h>
#include <string.h>

/* async scan the input until get \r */
int async_readline(char buf[MAX_SIZE], int size){
  unsigned int idx = 0;
  char c;
  do{
    c = async_uart_getc();
    /* After reboot, rpi3b+ will send non-ascii char, so we need to check it */
    if(c < 0 || c >= 128) continue;
    /* if get newline, then print \r\n and break */
    if(c == '\n'){
      async_uart_puts("\n");
      break;
    } 
    /* check the backspace character */
    else if(c == '\x7f' || c == '\b'){
      if(idx > 0){
        async_uart_putc('\b');
        async_uart_putc(' ');
        async_uart_putc('\b');
        idx--;
      }
    }
    /* otherwise, print and save the character */
    else{
      async_uart_putc(c); // need to recv the echo back
      if( idx < size){
        buf[idx++] = c;
      }
    }
  } while(1);
  buf[idx] = '\0';
  return idx;
}

/* scan the input until get \r */
int readline(char buf[MAX_SIZE], int size){
  unsigned int idx = 0;
  char c;
  do{
    c = uart_getc();
    /* After reboot, rpi3b+ will send non-ascii char, so we need to check it */
    if(c < 0 || c >= 128) continue;
    /* if get newline, then print \r\n and break */
    if(c == '\n'){
      uart_puts("\n");
      break;
    } 
    /* check the backspace character */
    else if(c == '\x7f' || c == '\b'){
      if(idx > 0){
        uart_putc('\b');
        uart_putc(' ');
        uart_putc('\b');
        idx--;
      }
    }
    /* otherwise, print and save the character */
    else{
      uart_putc(c); // need to recv the echo back
      if( idx < size){
        buf[idx++] = c;
      }
    }
  } while(1);
  buf[idx] = '\0';
  return idx;
}

/* read n bytes from uart */
int readnbyte(char buf[MAX_SIZE], int size){
  unsigned int idx = 0;
  char c;
  while(idx < size && idx < MAX_SIZE){
    c = uart_getc();
    buf[idx++] = c;
  }
  buf[idx] = '\0';
  return idx;
}