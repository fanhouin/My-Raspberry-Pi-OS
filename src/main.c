#include "uart.h"

int main(){
    uart_init();
    uart_puts("Hello World!\n");
    while(1){
        uart_send(uart_getc());
    }
    return 0;
}