#ifndef	EXC_H_
#define	EXC_H_
#include <syscall.h>

void exception_handler( unsigned long long, 
                        unsigned long long, 
                        unsigned long long,
                        TrapFrame *);

void syscall_handler(unsigned int syscall_id, TrapFrame *trapFrame);

#endif  

