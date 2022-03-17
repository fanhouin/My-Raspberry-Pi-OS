#include <uart.h>
#include <read.h>
#include <string.h>

/* bootloader entry point */
extern unsigned long _bootstart;

/* kernel entry point */
extern unsigned long _start;

/* kernel image size */
extern unsigned long __kernel_image_size;

unsigned long DTB_BASE = 0x123;
int begin = 1;

void self_relocation(){
    char *src = (char *)&_start;
    char *dst = (char *)0x60000;
    unsigned long size = (unsigned long)&__kernel_image_size;
    memcpy(dst, src, size);

    //Cast the address back to a function and call it.
    void (*func_ptr)() = (void *)&_bootstart;
    func_ptr();
}

int main(unsigned long dtb){
    register unsigned long x0 asm("x0");
    if(begin){
        DTB_BASE = x0;
        begin = 0;
        self_relocation();
    }

    uart_init();
    char buf[15];
    char *kernel = (char *)0x80000;

    memset(buf, '\0', 15);
    readline(buf, 15);
    uart_puts("[*] Kernel image size: ");
    int size = atoi(buf);
    uart_puts(buf);
    uart_puts("\n[*] Booting...\n");

    while(size--){
        *kernel++ = uart_getc();
    }


    void (*kernel_ptr)(unsigned long) = (void *)0x80000;
    kernel_ptr(DTB_BASE);

    return 0;
}