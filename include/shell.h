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

/* reboot device */
void Reboot();

/* read keyboard message until read newline character */
int readline(char *, int);

/* shell */
void ShellLoop();

#endif