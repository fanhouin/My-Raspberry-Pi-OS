#ifndef READ_H_
#define READ_H_

#include <string.h>

/* async scan the input until get \r */
int async_readline(char [MAX_SIZE], int);

/* scan the input until get \r */
int readline(char [MAX_SIZE], int);

/* read n bytes from uart */
int readnbyte(char [MAX_SIZE], int);
#endif