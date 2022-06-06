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
#include <irq.h>
#include <vfs.h>

extern char *global_dir;

/* print welcome message*/
void PrintWelcome(){
  uart_puts("******************************************************************\n");
  uart_puts("********************* Welcome to FanFan's OS *********************\n");
  uart_puts("******************************************************************\n");
  uart_puts(global_dir);
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
  uart_puts("cpio_ls      : list cpio directory contents\n");
  uart_puts("cpio_cat     : print cpio file's data\n");
  uart_puts("cpio_exec    : exec file in cpio\n");
  uart_puts("setTimeout   : set timeout for read\n");
  uart_puts("test_timeout : test timeout for read\n");
  uart_puts("ls           : list directory contents\n");
  uart_puts("cd           : change working directory\n");
  uart_puts("mkdir        : make directories\n");
  uart_puts("mount        : mount a filesystem\n");
  uart_puts("umount       : umount a filesystem\n");
  uart_puts("exec         : exec a file in filesystem\n");
}


/* print unknown command message*/
void PrintUnknown(char buf[MAX_SIZE]){
  uart_puts("Unknown command: ");
  uart_puts(buf);
  uart_puts("\n");
}

/* print board revision*/
void PrintRevision(char buf[MAX_SIZE]){
  unsigned int __attribute__((aligned(16))) mbox[36];
  unsigned int success = get_board_revision(mbox);
  if (success){
    print_string(UITOHEX, "Board Revision: 0x", mbox[5], 1);
  } else{
    uart_puts("Failed to get board revision\n");
  }
}

/* print memory info*/
void PrintMemory(char buf[MAX_SIZE]){
  unsigned int __attribute__((aligned(16))) mbox[36];
  unsigned int success = get_arm_memory(mbox);
  if (success){
    print_string(UITOHEX, "ARM Memory Base Address: 0x", mbox[5], 1);
    print_string(UITOHEX, "ARM Memory Size: 0x", mbox[6], 1);
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

void cpio_ls(){
  ls();
}

void cpio_cat(char buf[MAX_SIZE]){
  uart_puts("Filename: ");
  unsigned int size = readline(buf, MAX_SIZE);
  if (size == 0){
    return;
  }
  cat(buf);
}

void cpio_exec(char buf[MAX_SIZE]){
  uart_puts("Filename: ");
  unsigned int size = readline(buf, MAX_SIZE);
  if (size == 0){
    return;
  }
  int state = kernel_exec(buf);
  enable_irq();

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

  add_timer(timeout_print, timeout, message, 0);
}

void TestTimeOut(char buf[MAX_SIZE]){
  add_timer(timeout_print, 2, "[*] timeout: 2\n", 0);
  add_timer(timeout_print, 1, "[*] timeout: 1\n", 0);
  add_timer(timeout_print, 5, "[*] timeout: 5\n", 0);
  add_timer(timeout_print, 4, "[*] timeout: 4\n", 0);
  add_timer(timeout_print, 3, "[*] timeout: 3\n", 0);
  add_timer(timeout_print, 2, "[*] timeout: 2.1\n", 0); // test short expired time
  add_timer(timeout_print, 9, "[*] timeout: 9\n", 0);
  add_timer(timeout_print, 7, "[*] timeout: 7\n", 0);
  add_timer(timeout_print, 6, "[*] timeout: 6\n", 0);
  add_timer(timeout_print, 8, "[*] timeout: 8\n", 0);

}

void ls_arg(char *buf){
  char *path = strchr(buf, ' ') + 1;
  if(strlen(path) == 0){
    vfs_ls(NULL);
  }
  else{
    vfs_ls(path);
  }
}

void chdir_arg(char *buf){
  char *path = strchr(buf, ' ') + 1;
  if(strlen(path) == 0){
    vfs_chdir(NULL);
  }
  else{
    vfs_chdir(path);
  }
}

void mkdir_arg(char *buf){
  char *path = strchr(buf, ' ') + 1;
  if(strlen(path) == 0){
    vfs_mkdir(NULL);
  }
  else{
    vfs_mkdir(path);
  }
}

void mount_arg(char *buf){
  char *path_ptr = strchr(buf, ' ') + 1;
  char *fs_name_ptr = strchr(path_ptr, ' ') + 1;
  path_ptr[strlen(path_ptr) - strlen(fs_name_ptr) - 1] = '\0';

  char path[MAX_PATHNAME_LEN * 16 + 1]; 
  char fs_name[MAX_PATHNAME_LEN + 1];
  strcpy(path, path_ptr);
  strcpy(fs_name, fs_name_ptr);

  vfs_mount(path, fs_name);
}

void umount_arg(char *buf){
  char *path = strchr(buf, ' ') + 1;
  if(strlen(path) == 0){
    vfs_umount(NULL);
  }
  else{
    vfs_umount(path);
  }
}

void exec_arg(char *buf){
  char *path = strchr(buf, ' ') + 1;
  int state = kernel_exec(path);
  enable_irq();

  if(state < 0){
    uart_puts("[x] Failed to exec the file\n");
    return;
  }
}

/* Main Shell */
void ShellLoop(){
  char buf[MAX_SIZE];

  while(1){

    memset(buf, '\0', MAX_SIZE);
    unsigned int size = readline(buf, sizeof(buf));
    if (size == 0){
      uart_puts(global_dir);
      uart_puts("# ");
      continue;
    } 
    if(strcmp("help", buf) == 0) PrintHelp();
    else if(strcmp("hello", buf) == 0) uart_puts("Hello World!\n");
    else if(strcmp("reboot", buf) == 0) Reboot();
    else if(strcmp("revision", buf) == 0) PrintRevision(buf);
    else if(strcmp("memory", buf) == 0) PrintMemory(buf);
    else if(strcmp("bootimg", buf) == 0) Bootimg(buf);
    else if(strcmp("cpio_ls", buf) == 0) cpio_ls();
    else if(strcmp("cpio_cat", buf) == 0) cpio_cat(buf);
    else if(strcmp("cpio_exec", buf) == 0) cpio_exec(buf);
    else if(strncmp("setTimeout", buf, strlen("setTimeout")) == 0) SetTimeOut(buf);
    else if(strcmp("test_timeout", buf) == 0) TestTimeOut(buf);
    else if(strncmp("ls", buf, strlen("ls")) == 0) ls_arg(buf);
    else if(strncmp("cd", buf, strlen("cd")) == 0) chdir_arg(buf);
    else if(strncmp("mkdir", buf, strlen("mkdir")) == 0) mkdir_arg(buf);
    else if(strncmp("mount", buf, strlen("mount")) == 0) mount_arg(buf);
    else if(strncmp("umount", buf, strlen("umount")) == 0) umount_arg(buf);
    else if(strncmp("exec", buf, strlen("exec")) == 0) exec_arg(buf);
    else PrintUnknown(buf);
    
    
    uart_puts(global_dir);
    uart_puts("# ");
  }
    
}

