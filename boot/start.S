.section ".text.boot"
.global _start

_start:
    mrs     x1, mpidr_el1 // get system register
    and     x1, x1, #3 // mask out bits not belonging to core id
    cbz     x1, 2f // branch if result is zero (core id matched)

// cpu id > 0, stop
1:  wfe
    b       1b

// cpu id == 0
// Set stack to start below the .text section
2:  ldr     x1, =_start
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
5:  bl      main
    b       1b 