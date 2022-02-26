#include <shell.h>
#include <uart.h>
#include <string.h>


void PrintWelcome(){
  uart_puts("**************************************************\n");
  uart_puts("************* Welcome to FanFan's OS *************\n");
  uart_puts("**************************************************\n");
  uart_puts("# ");
}

void PrintHelp(){
  uart_puts("------------------ Help Message ------------------\n");
  uart_puts("help     : print this help menu\n");
  uart_puts("hello    : print Hello World!\n");
  uart_puts("reboot   : reboot the device\n");
}

int readline(char *buf, int size){
  int idx = 0;
  char c = uart_getc();
  uart_send(c);

  while(c != '\n'){
      if (c != '\r' && idx < size){
          buf[idx++] = c;
      }
      c = uart_getc();
      uart_send(c); // need to recv the echo back
  }
  buf[idx] = '\0';
  return 0;
}


void ShellLoop(){
  char buf[MAX_SIZE];
  while(1){
    readline(buf, sizeof(buf));
    if(strcmp("help", buf) == 0) PrintHelp();
    else if(strcmp("hello", buf) == 0) uart_puts("Hello World!\n");
    else if(strcmp("reboot", buf) == 0) uart_puts("Rebooting...\n");
    else uart_puts("Unknown command\n");

    uart_puts("# ");
  }
    
}

