#include <shell.h>
#include <uart.h>
#include <string.h>
#include <mailbox.h>


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

void PrintUnknown(char buf[MAX_SIZE]){
  uart_puts("Unknown command: ");
  uart_puts(buf);
  uart_puts("\n");
}

void PrintRevision(char buf[MAX_SIZE]){
  unsigned int mailbox[7];
  unsigned int statu = get_board_revision(mailbox);
  if (statu == REQUEST_SUCCEED){
    uart_puts("Board revision: 0x");
    uitohex(mailbox[5], buf);
    uart_puts(buf);
    uart_puts("\n");
  } else if(statu == REQUEST_FAILED){
    uart_puts("Failed to get board revision\n");
  }
}

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


void ShellLoop(){
  char buf[MAX_SIZE];
  while(1){
    unsigned int size = readline(buf, sizeof(buf));
    if (size == 0){
      uart_puts("# ");
      continue;
    } 
    if(strcmp("help", buf) == 0) PrintHelp();
    else if(strcmp("hello", buf) == 0) uart_puts("Hello World!\n");
    else if(strcmp("reboot", buf) == 0) uart_puts("Rebooting...\n");
    else if(strcmp("revision", buf) == 0) PrintRevision(buf);
    else PrintUnknown(buf);

    uart_puts("# ");
  }
    
}

