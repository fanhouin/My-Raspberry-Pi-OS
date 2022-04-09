#ifndef STRING_H_
#define STRING_H_

#define MAX_SIZE 1024

/* Compare S1 and S2, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
int strcmp(const char *, const char *);

/* string length */
unsigned int strlen(const char *);

/* reverse string */
void reverse_buf(char *);

/* int to string */
void itoa(int, char *);

/* uint to hex string */
void uitohex(unsigned int, char *);

/* array to int */
int atoi(const char *);

void memcpy(char *, const char *, unsigned int);
void memset(char *, const char, unsigned int);
void strcpy(char *, const char *);
void strcat(char *, const char *);

#endif 