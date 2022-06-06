#include <uart.h>
#include <string.h>
#include <user_syscall.h>
#include <mailbox.h>

int readline2(char buf[MAX_SIZE], int size){
  unsigned int idx = 0;
  char u[3];
  do{
    uartread(u, 1);
    /* After reboot, rpi3b+ will send non-ascii char, so we need to check it */
    if(u[0] < 0 || u[0] >= 128) continue;
    /* if get newline, then print \r\n and break */
    if(u[0] == '\n'){
      uartwrite("\n", 1);
      break;
    } 
    /* check the backspace character */
    else if(u[0] == '\x7f' || u[0] == '\b'){
      if(idx > 0){
        uartwrite("\b", 1);
        uartwrite(" ", 1);
        uartwrite("\b", 1);
        idx--;
      }
    }
    /* otherwise, print and save the character */
    else{
      uartwrite(u, 1); // need to recv the echo back
      if( idx < size){
        buf[idx++] = u[0];
      }
    }
  } while(1);
  buf[idx] = '\0';
  return idx;
}

void delay1(unsigned long long time){
    unsigned long long system_timer = 0;
    asm volatile(
        "mrs %0, cntpct_el0\n\t"
        :"=r"(system_timer)
    );
    unsigned long long expired_time = system_timer + time;
    while(system_timer <= expired_time){
        asm volatile(
            "mrs %0, cntpct_el0\n\t"
            :"=r"(system_timer)
        );
    }
    
}

int get_arm_memory1(unsigned int *mbox){
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

  return mbox_call(MAILBOX_CH_PROP, mbox);
}

int main(){
    uart_puts("----------------------------user program2----------------------------\n");
    print_string(UITOA, "[user] Fork Test, pid = ", getpid(), 1);
    int cnt = 1;
    int ret = 0;
    unsigned int mbox[36];
    int success = get_arm_memory1(mbox);

    if (success){
      print_string(UITOHEX, "[user] ARM Memory Base Address: 0x", mbox[5], 1);
      print_string(UITOHEX, "[user] ARM Memory Size: 0x", mbox[6], 1);
    } else{
      uart_puts("[user] Failed to get board revision\n");
    }

    if((ret = fork()) == 0){
        long long cur_sp;
        asm volatile("mov %0, sp" : "=r"(cur_sp));
        print_string(UITOA, "[user] first child pid: ", getpid(), 0);
        print_string(UITOA, " | cnt: ", cnt, 0);
        print_string(UITOHEX, " | cnt_ptr: 0x", (unsigned long long)&cnt, 0);
        print_string(UITOHEX, " | sp: 0x", cur_sp, 1);

        ++cnt;
        if ((ret = fork()) != 0){
            asm volatile("mov %0, sp" : "=r"(cur_sp));
            print_string(UITOA, "[user] first child pid: ", getpid(), 0);
            print_string(UITOA, " | cnt: ", cnt, 0);
            print_string(UITOHEX, " | cnt_ptr: 0x", (unsigned long long)&cnt, 0);
            print_string(UITOHEX, " | sp: 0x", cur_sp, 1);
        }
        else{
            while (cnt < 5) {
              asm volatile("mov %0, sp" : "=r"(cur_sp));
              print_string(UITOA, "[user] second child pid: ", getpid(), 0);
              print_string(UITOA, " | cnt: ", cnt, 0);
              print_string(UITOHEX, " | cnt_ptr: 0x", (unsigned long long)&cnt, 0);
              print_string(UITOHEX, " | sp: 0x", cur_sp, 1);
              delay1(100000000);
              ++cnt;
            }
        }
        exit(0);

    }else{
        print_string(UITOA, "[user] parent here, pid: ", getpid(), 0);
        print_string(UITOA, " | ret: ", ret, 1);
    }
    
    exit(0);
    
    return 0;
}

// int main(){
//     char buf[50];
//     memset(buf, 0, sizeof(buf));
//     // uart_init();
//     uartwrite("----------------------------user program----------------------------\n", 71);
//     int pid = getpid();
//     print_string(UITOA, "[user] pid = ", pid, 1);

//     uartwrite("[user] please enter the user program: ", 40);
//     unsigned int size = readline2(buf, 49);
//     print_string(UITOA, "[user] uart_read_size = ", size, 1);

//     size = uartwrite(buf, size);
//     print_string(UITOA, "[user] uart_write_size = ", size, 1);


//     char* argv[] = {"exec_test"};
//     int a = exec(buf, argv);
//     if(a == -1) 
//         uart_puts("[user] exec fail QQ\n");
    
//     exit();
    
//     return 0;
// }