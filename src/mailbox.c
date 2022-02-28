#include <mailbox.h>
#include <uart.h>


unsigned int get_board_revision(volatile unsigned int mbox[36]){
  mbox[0] = 7 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_BOARD_REVISION; // tag identifier
  mbox[3] = 4; // maximum of request and response value buffer's length.
  mbox[4] = TAG_REQUEST_CODE;
  mbox[5] = 0; // board revision
  // tags end
  mbox[6] = END_TAG;
  
  return mailbox_call(mbox, MAILBOX_CH_PROP); 
}

unsigned int get_arm_memory(volatile unsigned int mbox[36]){
  mbox[0] = 8 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_ARM_MEMORY; // tag identifier
  mbox[3] = 8; // maximum of request and response value buffer's length.
  mbox[4] = TAG_REQUEST_CODE;
  mbox[5] = 0; // base address in bytes
  mbox[6] = 0; // size in bytes
  // tags end
  mbox[7] = END_TAG;

  return mailbox_call(mbox, MAILBOX_CH_PROP);

}

/*
1. Combine the message address (upper 28 bits) with channel number (lower 4 bits)
2. Check if Mailbox 0 status register’s full flag is set.
3. If not, then you can write to Mailbox 1 Read/Write register.
4. Check if Mailbox 0 status register’s empty flag is set.
5. If not, then you can read from Mailbox 0 Read/Write register.
6. Check if the value is the same as you wrote in step 1.
*/
unsigned int mailbox_call(volatile unsigned int mbox[36], unsigned char ch){
  /* Combine the message address (upper 28 bits) with channel number (lower 4 bits) */
  unsigned int req = (((unsigned int)((unsigned long)mbox) & (~0xF)) | (ch & 0xF));
  /* wait until we can write to the mailbox */
  while(*MAILBOX_STATUS & MAILBOX_FULL){asm volatile("nop");}
  *MAILBOX_WRITE = req;

  /* now wait for the response */
  while(1){

    /* wait the response signal */
    while(*MAILBOX_STATUS & MAILBOX_EMPTY){asm volatile("nop");}
    
    /* read the response to compare the our req and request_code */
    if(req == *MAILBOX_READ){
      return mbox[1] == MAILBOX_RESPONSE;
    }
  }
  return 0;
}