#ifndef MAILBOX_H_
#define MAILBOX_H_
#include <gpio.h>

/* mailbox registers */
#define MAILBOX_BASE        (MMIO_BASE + 0xB880) // MMIO base address + MAILBOX offset
#define MAILBOX_READ        ((volatile unsigned int*)(MAILBOX_BASE))
#define MAILBOX_POLL        ((volatile unsigned int*)(MAILBOX_BASE+0x10))
#define MAILBOX_SENDER      ((volatile unsigned int*)(MAILBOX_BASE+0x14))
#define MAILBOX_STATUS0     ((volatile unsigned int*)(MAILBOX_BASE+0x18))
#define MAILBOX_STATUS1     ((volatile unsigned int*)(MAILBOX_BASE+0x38))
#define MAILBOX_CONFIG      ((volatile unsigned int*)(MAILBOX_BASE+0x1C))
#define MAILBOX_WRITE       ((volatile unsigned int*)(MAILBOX_BASE+0x20))

#define MAILBOX_EMPTY       0x40000000
#define MAILBOX_FULL        0x80000000
#define MAILBOX_RESPONSE    0x80000000

/* tags */
#define GET_BOARD_REVISION  0x00010002
#define GET_ARM_MEMORY      0x00010005
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

/* channels */
#define MAILBOX_CH_POWER    0
#define MAILBOX_CH_FB       1
#define MAILBOX_CH_VUART    2
#define MAILBOX_CH_VCHIQ    3
#define MAILBOX_CH_LEDS     4
#define MAILBOX_CH_BTNS     5
#define MAILBOX_CH_TOUCH    6
#define MAILBOX_CH_COUNT    7
#define MAILBOX_CH_PROP     8

unsigned int get_board_revision(unsigned int [36]);
unsigned int get_arm_memory(unsigned int [36]);
unsigned int mailbox_call(unsigned int *, unsigned char);

#endif
