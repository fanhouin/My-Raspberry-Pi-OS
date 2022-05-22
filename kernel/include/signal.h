#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <syscall.h>

void check_sig_queue(TrapFrame*);
void sig_default_handler();
void sig_register_handler();

#endif