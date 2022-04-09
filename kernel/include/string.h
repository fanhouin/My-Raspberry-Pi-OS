#ifndef STRING_H_
#define STRING_H_

#define MAX_SIZE 512

/* Compare S1 and S2, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, unsigned int);

/* string length */
unsigned int strlen(const char *);

/* reverse string */
void reverse_buf(char *);

/* int to string */
void itoa(char *, int);

/* uint to string */
void uitoa(char *, int);

/* uint to hex string */
void uitohex(char *, unsigned int);

/* array to int */
int atoi(const char *);

/* array to uint */
unsigned int atoui(const char *);

/* hex string to unsigned int */
unsigned int hextoui(const char *, unsigned int);

void memcpy(char *, const char *, unsigned int);
void memset(char *, const char, unsigned int);
void strcpy(char *, const char *);
void strcat(char *, const char *);
char *strchr(const char *, int);

#endif 