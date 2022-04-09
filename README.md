# Operating Systems Capstone 2022

|GitHub account name|Student ID|Name|
|---|---|---|
|fanhouin|310552001|樊浩賢|

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
 ┃ ┃ ┣ 📜cpio.h
 ┃ ┃ ┣ 📜exc.h
 ┃ ┃ ┣ 📜fdt.h
 ┃ ┃ ┣ 📜gpio.h
 ┃ ┃ ┣ 📜irq.h
 ┃ ┃ ┣ 📜mailbox.h
 ┃ ┃ ┣ 📜malloc.h
 ┃ ┃ ┣ 📜read.h
 ┃ ┃ ┣ 📜reboot.h
 ┃ ┃ ┣ 📜shell.h
 ┃ ┃ ┣ 📜stdint.h
 ┃ ┃ ┣ 📜string.h
 ┃ ┃ ┣ 📜task.h
 ┃ ┃ ┣ 📜timer.h
 ┃ ┃ ┗ 📜uart.h
 ┃ ┣ 📂initramfs
 ┃ ┃ ┣ 📜me
 ┃ ┃ ┣ 📜test1
 ┃ ┃ ┣ 📜test2.txt
 ┃ ┃ ┣ 📜test3.cc
 ┃ ┃ ┣ 📜user.S
 ┃ ┃ ┗ 📜user.img
 ┃ ┣ 📂src
 ┃ ┃ ┣ 📜cpio.c
 ┃ ┃ ┣ 📜exc.c
 ┃ ┃ ┣ 📜fdt.c
 ┃ ┃ ┣ 📜irq.c
 ┃ ┃ ┣ 📜mailbox.c
 ┃ ┃ ┣ 📜main.c
 ┃ ┃ ┣ 📜malloc.c
 ┃ ┃ ┣ 📜read.c
 ┃ ┃ ┣ 📜reboot.c
 ┃ ┃ ┣ 📜shell.c
 ┃ ┃ ┣ 📜string.c
 ┃ ┃ ┣ 📜task.c
 ┃ ┃ ┣ 📜timer.c
 ┃ ┃ ┗ 📜uart.c
 ┃ ┣ 📜Makefile
 ┃ ┣ 📜bcm2710-rpi-3-b-plus.dtb
 ┃ ┗ 📜initramfs.cpio
 ┣ 📜config.txt
 ┗ 📜uartboot.py
```