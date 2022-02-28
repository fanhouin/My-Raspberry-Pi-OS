#include <read.h>
#include <uart.h>
#include <string.h>


int readline(char buf[MAX_SIZE], int size){
  unsigned int idx = 0;
  char c = uart_getc();
  if (c == '\n') uart_send('\r');
  uart_send(c);

  while(c != '\n'){
      if (c != '\r' && idx < size){
          buf[idx++] = c;
      }
      c = uart_getc();
      if (c == '\n') uart_send('\r');
      uart_send(c); // need to recv the echo back
  }
  buf[idx] = '\0';
  return idx;
}