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

.global signal
signal:
    mov x8, SIGNAL_
    svc #0
    ret

.global signal_kill
signal_kill:
    mov x8, SIGKILL
    svc #0
    ret

.global sigreturn
sigreturn:
    mov x8, SIGRETURN
    svc #0
    ret

.global open
open:
    mov x8, OPEN
    svc #0
    ret

.global close
close:
    mov x8, CLOSE
    svc #0
    ret

.global write
write:
    mov x8, WRITE
    svc #0
    ret

.global read
read:
    mov x8, READ
    svc #0
    ret

.global mkdir
mkdir:
    mov x8, MKDIR
    svc #0
    ret

.global mount
mount:
    mov x8, MOUNT
    svc #0
    ret

.global chdir
chdir:
    mov x8, CHDIR
    svc #0
    ret

.global lseek64
lseek64:
    mov x8, LSEEK64
    svc #0
    ret

.global ioctl
    mov x8, IOCTL
    svc #0
    ret
