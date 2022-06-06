#include <shell.h>
#include <uart.h>
#include <string.h>
#include <mailbox.h>
#include <reboot.h>
#include <read.h>
#include <cpio.h>
#include <timer.h>
#include <malloc.h>
#include <syscall.h>

/* print welcome message*/
void PrintWelcome(){
  uart_puts("******************************************************************\n");
  uart_puts("********************* Welcome to FanFan's OS *********************\n");
  uart_puts("******************************************************************\n");
  uart_puts("# ");
}

/* print help message*/
void PrintHelp(){
  uart_puts("-------------------------- Help Message --------------------------\n");
  uart_puts("help         : print this help menu\n");
  uart_puts("hello        : print Hello World!\n");
  uart_puts("revision     : print board_revision\n");
  uart_puts("memory       : print memory info\n");
  uart_puts("reboot       : reboot the device\n");
  uart_puts("ls           : list directory contents\n");
  uart_puts("exec         : concatenate files and print on the standard output\n");
  uart_puts("setTimeout   : set timeout for read\n");
  uart_puts("test_timeout : test timeout for read\n");
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
    uitohex(buf, mbox[5]);
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
    uitohex(buf, mbox[5]);
    uart_puts(buf);
    uart_puts("\n");

    uart_puts("ARM Memory Size: 0x");
    uitohex(buf, mbox[6]);
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

/* uart booting */
void Bootimg(char buf[MAX_SIZE]){
  memset(buf, '\0', MAX_SIZE);
  readnbyte(buf, 10);
  unsigned int size = atoi(buf);
  uart_puts("kernal image size: ");
  uart_puts(buf);
  uart_puts("\n");

  memset(buf, '\0', MAX_SIZE);
  readnbyte(buf, size);

  uart_puts("done\n");
}

void Ls(){
  ls();
}

void Cat(char buf[MAX_SIZE]){
  uart_puts("Filename: ");
  unsigned int size = readline(buf, MAX_SIZE);
  if (size == 0){
    return;
  }
  cat(buf);
}

void Exec(char buf[MAX_SIZE]){
  uart_puts("Filename: ");
  unsigned int size = readline(buf, MAX_SIZE);
  if (size == 0){
    return;
  }
  int state = kernel_exec(buf);

  if(state == -1){
    uart_puts("[x] Failed to exec the file\n");
    return;
  }
}

void SetTimeOut(char buf[MAX_SIZE]){
  char *message_tmp = strchr(buf, ' ') + 1;
  char *end_message = strchr(message_tmp, ' ');
  *end_message = '\0';
  char *message = (char *)simple_malloc(strlen(message_tmp) + 1);
  strcpy(message, message_tmp);
  unsigned int timeout = atoui(end_message + 1);

  add_timer(timeout_print, timeout, message);
}

void TestTimeOut(char buf[MAX_SIZE]){
  add_timer(timeout_print, 2, "[*] timeout: 2\n");
  add_timer(timeout_print, 1, "[*] timeout: 1\n");
  add_timer(timeout_print, 5, "[*] timeout: 5\n");
  add_timer(timeout_print, 4, "[*] timeout: 4\n");
  add_timer(timeout_print, 3, "[*] timeout: 3\n");
  add_timer(timeout_print, 2, "[*] timeout: 2.1\n"); // test short expired time
  add_timer(timeout_print, 9, "[*] timeout: 9\n");
  add_timer(timeout_print, 7, "[*] timeout: 7\n");
  add_timer(timeout_print, 6, "[*] timeout: 6\n");
  add_timer(timeout_print, 8, "[*] timeout: 8\n");

}

/* Main Shell */
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
    else if(strcmp("bootimg", buf) == 0) Bootimg(buf);
    else if(strcmp("ls", buf) == 0) Ls();
    else if(strcmp("cat", buf) == 0) Cat(buf);
    else if(strcmp("exec", buf) == 0) Exec(buf);
    else if(strncmp("setTimeout", buf, strlen("setTimeout")) == 0) SetTimeOut(buf);
    else if(strcmp("test_timeout", buf) == 0) TestTimeOut(buf);
    else PrintUnknown(buf);

    uart_puts("# ");
  }
    
}

