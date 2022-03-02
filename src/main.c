#include <uart.h>
#include <shell.h>

int main(){
    uart_init();
    PrintWelcome();
    ShellLoop();
    
    return 0;
}