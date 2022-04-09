.section ".text.boot"
.global _start

_start:
    mov     x10, x0
    mrs     x1, mpidr_el1 // get system register
    and     x1, x1, #3 // mask out bits not belonging to core id
    cbz     x1, 2f // branch if result is zero (core id matched)

// cpu id > 0, stop
1:  wfe
    b       1b

// cpu id == 0
// Set stack to start below the .text section
2:  bl      set_exception_vector_table
    bl      from_el2_to_el1
    
    // bl      core_timer_handler

    ldr     x1, =_start
    mov     sp, x1
    
// Get bss info
3:  ldr     x1, =__bss_start
    ldr     w2, =__bss_size

// Set bss to zero
4:  cbz     w2, 5f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 4b

// Run main()
5:  mov     x0, x10
    bl      main
    b       1b 


// save general registers to stack
.macro save_all
    sub sp, sp, 33 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]

    mrs x5, spsr_el1
    mrs x6, elr_el1
    stp x5, x6, [sp ,16 * 16]
.endm

// load general registers from stack
.macro load_all
    ldp x5, x6, [sp ,16 * 16]
    msr spsr_el1, x5 
    msr elr_el1, x6

    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]

    add sp, sp, 33 * 8
.endm

sync_exception_entry:
    save_all
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    bl      exception_handler
    load_all
    eret

irq_entry:
    save_all
    mrs     x1, spsr_el1
    bl      irq_handler
    load_all
    eret

fail:
    save_all
    b fail
    load_all
    eret


.align 11 // vector table should be aligned to 0x800
exception_vector_table:
    b fail // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    b fail
    .align 7
    b fail
    .align 7
    b fail
    .align 7

    b fail
    .align 7
    b irq_entry
    .align 7
    b fail
    .align 7
    b fail
    .align 7

    b sync_exception_entry
    .align 7
    b irq_entry
    .align 7
    b fail
    .align 7
    b fail
    .align 7

    b fail
    .align 7
    b fail
    .align 7
    b fail
    .align 7
    b fail
    .align 7

set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret

from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr
    eret // return to EL1

/* 
#define CORE0_TIMER_IRQ_CTRL 0x40000040
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1] // unmask timer interrupt
    ret
*/


