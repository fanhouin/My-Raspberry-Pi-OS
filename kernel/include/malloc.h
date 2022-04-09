#ifndef MALLOC_H_
#define MALLOC_H_

#define MALLOC_BASE   ((volatile unsigned long*)(0x2000000))

void *simple_malloc(unsigned long);

#endif