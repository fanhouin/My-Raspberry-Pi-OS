#ifndef SHELL_H_
#define SHELL_H_
#include <string.h>

/* print welcome message*/
void PrintWelcome();

/* print help message*/
void PrintHelp();

/* print unknown command message*/
void PrintUnknown(char [MAX_SIZE]);

/* print board revision*/
void PrintRevision(char [MAX_SIZE]);

/* print memory info*/
void PrintMemory(char [MAX_SIZE]);

/* uart booting */
void Bootimg(char [MAX_SIZE]);

/* reboot device */
void Reboot();

/* Main Shell */
void ShellLoop();

void cpio_ls();
void cpio_cat(char [MAX_SIZE]);
void cpio_exec(char [MAX_SIZE]);

void SetTimeOut(char [MAX_SIZE]);
void TestTimeOut(char [MAX_SIZE]);

void ls_arg(char *);
void chdir_arg(char *);
void mkdir_arg(char *);
void mount_arg(char *);
void umount_arg(char *);
void exec_arg(char *);

#endif