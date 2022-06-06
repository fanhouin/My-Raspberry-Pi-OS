#include <uart.h>
#include <shell.h>
#include <cpio.h>
#include <malloc.h>
#include <string.h>
#include <fdt.h>
#include <irq.h>
#include <allocator.h>
#include <task.h>
#include <sched.h>
#include <syscall.h>
#include <vfs.h>
#include <test_fs.h>
#include <timer.h>
#include <mailbox.h>

extern Thread *run_thread_head;

int main(unsigned long dtb_base){

    uart_init();
    enable_el0_get_timer();
    // uart_getc();
    print_string(UITOHEX, "[*] DTB_BASE: 0x", dtb_base, 1);
    fdt_traverse((fdt_header *)dtb_base, initramfs_callback);
    framebuffer_init();
    all_allocator_init();    
    init_cpio_file_info();
    rootfs_init("rootfs");
    init_thread_pool_and_head();
    init_task_head();

    enable_timer_irq();
    enable_irq(); // DAIF set to 0b0000

    // kernel_exec("vfs1.img");
    // vfs_load_program("vfs1.img");
    // fs_test1();
    // fs_test2();
    // fs_test3();
    // fs_test4();
    // fs_test5();
    // fs_test6();
    // fs_test7();
    // user_basic1();
    // user_advance1();

    PrintWelcome();
    ShellLoop();
    
    return 0;
}