#include <uart.h>
#include <shell.h>

int main(){
    uart_init();
    PrintWelcome();
    ShellLoop();

    // while(1){
    //     uart_send(uart_getc());
    // }
    return 0;
}