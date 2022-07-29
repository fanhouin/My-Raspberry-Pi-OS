# Operating Systems Capstone 2022

## Run with QEMU
### Prerequisites
```bash
sudo apt-get install gcc-aarch64-linux-gnu
sudo apt-get install qemu-system-aarch64
```
### Make kernel image
```bash
cd kernel
make
```

### Make bootloader image
```bash
cd bootloader
make
```

## Run
###  Just run kernel image
```bash
cd kernel
make run
```
### Boot kernel image with bootloader
```bash
# terminal 1
cd bootloader
make pty

# terminal 2
sudo screen /dev/pts/<num> 115200

# terminal 3 
cd kernel
sudo ../uartboot.py <num>
```
## Directory
```
📦osc2022
 ┣ 📂bootloader
 ┃ ┣ 📂boot
 ┃ ┃ ┣ 📜linker.ld
 ┃ ┃ ┗ 📜start.S
 ┃ ┣ 📂include
 ┃ ┃ ┣ 📜gpio.h
 ┃ ┃ ┣ 📜read.h
 ┃ ┃ ┣ 📜string.h
 ┃ ┃ ┗ 📜uart.h
 ┃ ┣ 📂src
 ┃ ┃ ┣ 📜main.c
 ┃ ┃ ┣ 📜read.c
 ┃ ┃ ┣ 📜string.c
 ┃ ┃ ┗ 📜uart.c
 ┃ ┣ 📜Makefile
 ┃ ┣ 📜bcm2710-rpi-3-b-plus.dtb
 ┃ ┗ 📜initramfs.cpio
 ┣ 📂kernel
 ┃ ┣ 📂boot
 ┃ ┃ ┣ 📜linker.ld
 ┃ ┃ ┗ 📜start.S
 ┃ ┣ 📂include
 ┃ ┃ ┣ 📜allocator.h
 ┃ ┃ ┣ 📜cpio.h
 ┃ ┃ ┣ 📜dev_ops.h
 ┃ ┃ ┣ 📜exc.h
 ┃ ┃ ┣ 📜fdt.h
 ┃ ┃ ┣ 📜gpio.h
 ┃ ┃ ┣ 📜irq.h
 ┃ ┃ ┣ 📜list.h
 ┃ ┃ ┣ 📜mailbox.h
 ┃ ┃ ┣ 📜malloc.h
 ┃ ┃ ┣ 📜math.h
 ┃ ┃ ┣ 📜read.h
 ┃ ┃ ┣ 📜reboot.h
 ┃ ┃ ┣ 📜sched.h
 ┃ ┃ ┣ 📜shell.h
 ┃ ┃ ┣ 📜signal.h
 ┃ ┃ ┣ 📜stddef.h
 ┃ ┃ ┣ 📜stdint.h
 ┃ ┃ ┣ 📜string.h
 ┃ ┃ ┣ 📜syscall.h
 ┃ ┃ ┣ 📜task.h
 ┃ ┃ ┣ 📜test_fs.h
 ┃ ┃ ┣ 📜timer.h
 ┃ ┃ ┣ 📜tmpfs.h
 ┃ ┃ ┣ 📜uart.h
 ┃ ┃ ┣ 📜user_syscall.h
 ┃ ┃ ┗ 📜vfs.h
 ┃ ┣ 📂initramfs
 ┃ ┃ ┣ 📂flag
 ┃ ┃ ┃ ┣ 📂fffflag
 ┃ ┃ ┃ ┃ ┗ 📜flag   
 ┃ ┃ ┃ ┗ 📜fakeflag.txt
 ┃ ┃ ┣ 📜me
 ┃ ┃ ┣ 📜test1
 ┃ ┃ ┣ 📜test2.txt
 ┃ ┃ ┣ 📜test3.cc
 ┃ ┃ ┗ 📜user.S
 ┃ ┣ 📂src
 ┃ ┃ ┣ 📜allocator.c
 ┃ ┃ ┣ 📜cpio.c
 ┃ ┃ ┣ 📜ctx_switch.S
 ┃ ┃ ┣ 📜dev_ops.c
 ┃ ┃ ┣ 📜exc.c
 ┃ ┃ ┣ 📜fdt.c
 ┃ ┃ ┣ 📜irq.c
 ┃ ┃ ┣ 📜mailbox.c
 ┃ ┃ ┣ 📜main.c
 ┃ ┃ ┣ 📜malloc.c
 ┃ ┃ ┣ 📜math.c
 ┃ ┃ ┣ 📜read.c
 ┃ ┃ ┣ 📜reboot.c
 ┃ ┃ ┣ 📜sched.c
 ┃ ┃ ┣ 📜shell.c
 ┃ ┃ ┣ 📜signal.c
 ┃ ┃ ┣ 📜string.c
 ┃ ┃ ┣ 📜syscall.c
 ┃ ┃ ┣ 📜task.c
 ┃ ┃ ┣ 📜test_fs.c
 ┃ ┃ ┣ 📜timer.c
 ┃ ┃ ┣ 📜tmpfs.c
 ┃ ┃ ┣ 📜uart.c
 ┃ ┃ ┣ 📜user_syscall.S
 ┃ ┃ ┗ 📜vfs.c
 ┃ ┣ 📜Makefile
 ┃ ┣ 📜bcm2710-rpi-3-b-plus.dtb
 ┃ ┗ 📜initramfs.cpio
 ┣ 📜config.txt
 ┗ 📜uartboot.py
```
