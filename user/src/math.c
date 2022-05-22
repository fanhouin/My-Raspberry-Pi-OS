#include <math.h>

unsigned int log(unsigned int  base, unsigned int  num){
    unsigned int  val = 0;
    unsigned int  tmp = 1;
    while(tmp < num){
        tmp *= base;
        val++;
    }
    return val;
}

unsigned int  log2(unsigned int  num){
    return log(2, num);
}