#include <shell.h>
#include <uart.h>
#include <string.h>
#include <mailbox.h>
#include <reboot.h>
#include <read.h>

/* print welcome message*/
void PrintWelcome(){
  uart_puts("**************************************************\n");
  uart_puts("************* Welcome to FanFan's OS *************\n");
  uart_puts("**************************************************\n");
  uart_puts("# ");
}

/* print help message*/
void PrintHelp(){
  uart_puts("------------------ Help Message ------------------\n");
  uart_puts("help     : print this help menu\n");
  uart_puts("hello    : print Hello World!\n");
  uart_puts("revision : print board_revision\n");
  uart_puts("memory   : print memory info\n");
  uart_puts("reboot   : reboot the device\n");
}

/* print unknown command message*/
void PrintUnknown(char buf[MAX_SIZE]){
  uart_puts("Unknown command: ");
  uart_puts(buf);
  uart_puts("\n");
}

/* print board revision*/
void PrintRevision(char buf[MAX_SIZE]){
  volatile unsigned int mbox[36];
  unsigned int success = get_board_revision(mbox);
  if (success){
    uart_puts("Board Revision: 0x");
    uitohex(mbox[5], buf);
    uart_puts(buf);
    uart_puts("\n");
  } else{
    uart_puts("Failed to get board revision\n");
  }
}

/* print memory info*/
void PrintMemory(char buf[MAX_SIZE]){
  volatile unsigned int mbox[36];
  unsigned int success = get_arm_memory(mbox);
  if (success){
    uart_puts("ARM Memory Base Address: 0x");
    uitohex(mbox[5], buf);
    uart_puts(buf);
    uart_puts("\n");

    uart_puts("ARM Memory Base Size: 0x");
    uitohex(mbox[6], buf);
    uart_puts(buf);
    uart_puts("\n");
  } else{
    uart_puts("Failed to get board revision\n");
  }
}

/* reboot device */
void Reboot(){
  uart_puts("Rebooting...\n");
  reset(100);
  while(1);
}

void ShellLoop(){
  char buf[MAX_SIZE];
  
  while(1){
    memset(buf, '\0', MAX_SIZE);
    unsigned int size = readline(buf, sizeof(buf));
    if (size == 0){
      uart_puts("# ");
      continue;
    } 
    if(strcmp("help", buf) == 0) PrintHelp();
    else if(strcmp("hello", buf) == 0) uart_puts("Hello World!\n");
    else if(strcmp("reboot", buf) == 0) Reboot();
    else if(strcmp("revision", buf) == 0) PrintRevision(buf);
    else if(strcmp("memory", buf) == 0) PrintMemory(buf);
    else PrintUnknown(buf);

    uart_puts("# ");
  }
    
}

