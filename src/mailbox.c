#include <mailbox.h>
#include <uart.h>


unsigned int get_board_revision(unsigned int mailbox[7]){
  mailbox[0] = 7 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_BOARD_REVISION; // tag identifier
  mailbox[3] = 4; // maximum of request and response value buffer's length.
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0; // value buffer
  // tags end
  mailbox[6] = END_TAG;

  return mailbox_call(mailbox, MAILBOX_CH_PROP); 
  // printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
}

void get_arm_memory(){
  // unsigned int mailbox[8];
  // mailbox[0] = 8 * 4; // buffer size in bytes
  // mailbox[1] = REQUEST_CODE;
  // // tags begin
  // mailbox[2] = GET_ARM_MEMORY; // tag identifier
  // mailbox[3] = 8; // maximum of request and response value buffer's length.
  // mailbox[4] = TAG_REQUEST_CODE;
  // mailbox[5] = 0; // value buffer
  // mailbox[6] = 0; // value buffer
  // // tags end
  // mailbox[7] = END_TAG;

  // return mailbox_call(mailbox, MAILBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.

  // printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
}

/*
1. Combine the message address (upper 28 bits) with channel number (lower 4 bits)
2. Check if Mailbox 0 status register’s full flag is set.
3. If not, then you can write to Mailbox 1 Read/Write register.
4. Check if Mailbox 0 status register’s empty flag is set.
5. If not, then you can read from Mailbox 0 Read/Write register.
6. Check if the value is the same as you wrote in step 1.
*/
unsigned int mailbox_call(unsigned int *mailbox, unsigned char ch){
  /* Combine the message address (upper 28 bits) with channel number (lower 4 bits) */
  unsigned int req = (((unsigned int)((unsigned long)mailbox) & 0xFFFFFFF0) | (ch & 0xF));
  /* wait until we can write to the mailbox */
  while(*MAILBOX_STATUS & MAILBOX_FULL){asm volatile("nop");}
  *MAILBOX_WRITE = *mailbox;

  /* now wait for the response */
  while(1){
    uart_puts("done\n");
    

    /* wait the response signal */
    while(*MAILBOX_STATUS & MAILBOX_EMPTY){asm volatile("nop");}
    /* read the response to compare the our req and request_code*/
    uart_puts("done\n");

    if(req == *MAILBOX_READ && mailbox[1] == REQUEST_CODE)
      return REQUEST_SUCCEED;
  }
  return REQUEST_FAILED;
}