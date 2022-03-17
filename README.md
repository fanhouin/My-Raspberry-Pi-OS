# Operating Systems Capstone 2022

|GitHub account name|Student ID|Name|
|---|---|---|
|fanhouin|310552001|樊浩賢|

## Build
### Make kernel image
```
> make
```

### Make bootloader image
```
> cd bootloader
> make
```

## Run
###  Just run kernel image
```
> make run
```
### Boot kernel image with bootloader
```
/* terminal 1 */
> cd bootloader
> make pty

/* terminal 2 */
> sudo screen /dev/pts/<num> 115200

/* terminal 3 */
> sudo ./uartboot.py <num>
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
 ┣ 📂boot
 ┃ ┣ 📜linker.ld
 ┃ ┗ 📜start.S
 ┣ 📂include
 ┃ ┣ 📜cpio.h
 ┃ ┣ 📜fdt.h
 ┃ ┣ 📜gpio.h
 ┃ ┣ 📜mailbox.h
 ┃ ┣ 📜malloc.h
 ┃ ┣ 📜read.h
 ┃ ┣ 📜reboot.h
 ┃ ┣ 📜shell.h
 ┃ ┣ 📜stdint.h
 ┃ ┣ 📜string.h
 ┃ ┗ 📜uart.h
 ┣ 📂rootfs
 ┃ ┣ 📜me
 ┃ ┣ 📜test1
 ┃ ┣ 📜test2.txt
 ┃ ┗ 📜test3.cc
 ┣ 📂src
 ┃ ┣ 📜cpio.c
 ┃ ┣ 📜fdt.c
 ┃ ┣ 📜mailbox.c
 ┃ ┣ 📜main.c
 ┃ ┣ 📜malloc.c
 ┃ ┣ 📜read.c
 ┃ ┣ 📜reboot.c
 ┃ ┣ 📜shell.c
 ┃ ┣ 📜string.c
 ┃ ┗ 📜uart.cS
 ┣ 📜bcm2710-rpi-3-b-plus.dtb
 ┣ 📜config.txt
 ┣ 📜initramfs.cpio
 ┗ 📜uartboot.py
```
