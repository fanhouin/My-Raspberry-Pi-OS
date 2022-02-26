#ifndef SHELL_H_
#define SHELL_H_

/* print welcome message*/
void PrintWelcome();

/* print help message*/
void PrintHelp();

/* read keyboard message until read newline character */
int readline(char *, int);

/* shell */
void ShellLoop();

#endif