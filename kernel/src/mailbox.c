#include <mailbox.h>
#include <uart.h>
#include <string.h>

unsigned int __attribute__((aligned(16))) framebuf_mbox[36];
unsigned int width, height, pitch, isrgb; /* dimensions and channel order */
unsigned char *lfb;                       /* raw frame buffer address */

unsigned int get_board_revision(unsigned int mbox[36]){
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

unsigned int get_arm_memory(unsigned int mbox[36]){
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

void framebuffer_init(){
  framebuf_mbox[0] = 35 * 4;
  framebuf_mbox[1] = MBOX_REQUEST;

  framebuf_mbox[2] = 0x48003; // set phy wh
  framebuf_mbox[3] = 8;
  framebuf_mbox[4] = 8;
  framebuf_mbox[5] = 1024; // FrameBufferInfo.width
  framebuf_mbox[6] = 768;  // FrameBufferInfo.height

  framebuf_mbox[7] = 0x48004; // set virt wh
  framebuf_mbox[8] = 8;
  framebuf_mbox[9] = 8;
  framebuf_mbox[10] = 1024; // FrameBufferInfo.virtual_width
  framebuf_mbox[11] = 768;  // FrameBufferInfo.virtual_height

  framebuf_mbox[12] = 0x48009; // set virt offset
  framebuf_mbox[13] = 8;
  framebuf_mbox[14] = 8;
  framebuf_mbox[15] = 0; // FrameBufferInfo.x_offset
  framebuf_mbox[16] = 0; // FrameBufferInfo.y.offset

  framebuf_mbox[17] = 0x48005; // set depth
  framebuf_mbox[18] = 4;
  framebuf_mbox[19] = 4;
  framebuf_mbox[20] = 32; // FrameBufferInfo.depth

  framebuf_mbox[21] = 0x48006; // set pixel order
  framebuf_mbox[22] = 4;
  framebuf_mbox[23] = 4;
  framebuf_mbox[24] = 1; // RGB, not BGR preferably

  framebuf_mbox[25] = 0x40001; // get framebuffer, gets alignment on request
  framebuf_mbox[26] = 8;
  framebuf_mbox[27] = 8;
  framebuf_mbox[28] = 4096; // FrameBufferInfo.pointer
  framebuf_mbox[29] = 0;    // FrameBufferInfo.size

  framebuf_mbox[30] = 0x40008; // get pitch
  framebuf_mbox[31] = 4;
  framebuf_mbox[32] = 4;
  framebuf_mbox[33] = 0; // FrameBufferInfo.pitch

  framebuf_mbox[34] = MBOX_TAG_LAST;

  // this might not return exactly what we asked for, could be
  // the closest supported resolution instead
  if (mailbox_call(framebuf_mbox, MBOX_CH_PROP) && framebuf_mbox[20] == 32 && framebuf_mbox[28] != 0) {
    framebuf_mbox[28] &= 0x3FFFFFFF; // convert GPU address to ARM address
    width = framebuf_mbox[5];        // get actual physical width
    height = framebuf_mbox[6];       // get actual physical height
    pitch = framebuf_mbox[33];       // get number of bytes per line
    isrgb = framebuf_mbox[24];       // get the actual channel order
    lfb = (void *)((unsigned long)framebuf_mbox[28]);
  } else {
    uart_puts("Unable to set screen resolution to 1024x768x32\n");
  }
}

/*
1. Combine the message address (upper 28 bits) with channel number (lower 4 bits)
2. Check if Mailbox 0 status register’s full flag is set.
3. If not, then you can write to Mailbox 1 Read/Write register.
4. Check if Mailbox 0 status register’s empty flag is set.
5. If not, then you can read from Mailbox 0 Read/Write register.
6. Check if the value is the same as you wrote in step 1.
*/
unsigned int mailbox_call(unsigned int *mbox, unsigned char ch){
  /* Combine the message address (upper 28 bits) with channel number (lower 4 bits) */
  unsigned int req = (((unsigned int)((unsigned long)mbox) & (~0xF)) | (ch & 0xF));
  /* wait until we can write to the mailbox */
  while(*MAILBOX_STATUS1 & MAILBOX_FULL){asm volatile("nop");}
  *MAILBOX_WRITE = req;

  /* now wait for the response */
  while(1){
    /* wait the response signal */
    while(*MAILBOX_STATUS0 & MAILBOX_EMPTY){asm volatile("nop");}

    /* read the response to compare the our req and request_code */
    if(req == *MAILBOX_READ){
      return mbox[1] == MAILBOX_RESPONSE;
    }
  }
  return 0;
}