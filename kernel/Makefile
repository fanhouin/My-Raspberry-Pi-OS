CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
INCLUDE = -I ./include/

SRC_DIR = src
BUILD_DIR = build
BOOT_DIR = boot

SFILES_ASM = $(wildcard src/*.S)
OFILES_ASM = $(patsubst src/%.S, build/%.o, $(SFILES_ASM)) # OFILES = $(SFILES:.c=.o)
SFILES = $(wildcard src/*.c)
OFILES = $(patsubst src/%.c, build/%.o, $(SFILES)) # OFILES = $(SFILES:.c=.o)
BOOTFILES = $(wildcard boot/*.S)
BOOTOFILES = $(patsubst boot/%.S, build/%.o, $(BOOTFILES)) # BOOTOFILES = $(BOOTFILES:.S=.o)
ALLOFILES = $(OFILES) $(BOOTOFILES) $(OFILES_ASM)

CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img run-display

$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.S
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -g 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	$(CC) $(CFLAGS)	$(INCLUDE) -c $< -o $@ -g 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS)	$(INCLUDE) -c $< -o $@ -g 

kernel8.img: $(ALLOFILES)
	$(LD) -nostdlib -nostartfiles  $(ALLOFILES) -T boot/linker.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm *.elf *.img build/* >/dev/null 2>/dev/null || true

run: 
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb 

run-display: 
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -s -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb

run-disk:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -drive if=sd,file=kernel8.img,format=raw,

pty: 
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -serial null -serial pty -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb

cpio:
	cd initramfs;\find . | cpio -o -H newc > ../initramfs.cpio;

user:
	cd ../user;\
	make;\
	cp user2.img ./initramfs/user2.img;\
	cd initramfs;\find . | cpio -o -H newc > ../initramfs.cpio;\
	cd ..;\
	cp ./initramfs.cpio ../kernel/initramfs.cpio
