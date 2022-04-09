#ifndef READ_H_
#define READ_H_

#include <string.h>

/* scan the input until get \r */
int readline(char [MAX_SIZE], int);

/* read n bytes from uart */
int readnbyte(char [MAX_SIZE], int);
#endif