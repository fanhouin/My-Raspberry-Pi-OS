#include <malloc.h>
#include <uart.h>
#include <string.h>


void *simple_malloc(unsigned long size) {
    static void *head = (void *)MALLOC_BASE;
    void *ptr = head;
    head = head + size;
    return ptr;
}