#ifndef	TIMER_H_
#define TIMER_H_

#include <gpio.h>
#include <stddef.h>

// Timer interrupt
#define TIMER_CS        ((volatile unsigned int*)(MMIO_BASE+0x00003000))
#define TIMER_CLO       ((volatile unsigned int*)(MMIO_BASE+0x00003004))
#define TIMER_CHI       ((volatile unsigned int*)(MMIO_BASE+0x00003008))
#define TIMER_C0        ((volatile unsigned int*)(MMIO_BASE+0x0000300C))
#define TIMER_C1        ((volatile unsigned int*)(MMIO_BASE+0x00003010))
#define TIMER_C2        ((volatile unsigned int*)(MMIO_BASE+0x00003014))
#define TIMER_C3        ((volatile unsigned int*)(MMIO_BASE+0x00003018))

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)


typedef void (*TimerTask)(void *);

typedef struct _Timer{
    TimerTask task;
    unsigned long long expired_time;
    void *args;
    struct _Timer *next;
    struct _Timer *prev;
}Timer;

void add_timer(TimerTask, unsigned long long, void *);
void timeout_print(void *);
void timer_interrupt_handler();
void timer_interrupt_handler_el0();

#endif