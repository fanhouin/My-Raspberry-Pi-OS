#define __ASSEMBLY__
#include "user_syscall.h"
.section ".text"

.global getpid
getpid:
    mov x8, GET_PID
    svc #0
    ret

.global uartread
uartread:
    mov x8, UART_READ
    svc #0
    ret

.global uartwrite
uartwrite:
    mov x8, UART_WRITE
    svc #0
    ret

.global exec
exec:
    mov x8, EXEC
    svc #0
    ret

.global fork
fork:
    mov x8, FORK
    svc #0
    ret

.global exit
exit:
    mov x8, EXIT
    svc #0
    ret

.global mbox_call
mbox_call:
    mov x8, MBOX_CALL
    svc #0
    ret

.global kill
kill:
    mov x8, KILL
    svc #0
    ret