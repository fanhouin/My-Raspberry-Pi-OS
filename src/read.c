#include <read.h>
#include <uart.h>
#include <string.h>

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
        uart_puts("\b");
        uart_puts(" ");
        uart_puts("\b");
        idx--;
      }
    }
    /* otherwise, print and save the character */
    else{
      uart_send(c); // need to recv the echo back
      if( idx < size){
        buf[idx++] = c;
      }
    }
  } while(1);
  buf[idx] = '\0';
  return idx;
}