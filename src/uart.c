#include "uart.h"
#include "gpio.h"

void uart_init(){
    *AUX_ENABLE |= 1;
    *AUX_MU_CNTL = 3;
    *AUX_MU_IER = 0;
    *AUX_MU_LCR = 3;
    *AUX_MU_MCR = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR = 6;
}