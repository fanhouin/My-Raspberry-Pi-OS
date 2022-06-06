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

all: clean user2.img

$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.S
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -g 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	$(CC) $(CFLAGS)	$(INCLUDE) -c $< -o $@ -g 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS)	$(INCLUDE) -c $< -o $@ -g 

user2.img: $(ALLOFILES)
	$(LD) -nostdlib -nostartfiles  $(ALLOFILES) -T boot/linker.ld -o user2.elf
	$(OBJCOPY) -O binary user2.elf user2.img

clean:
	rm *.elf *.img build/* >/dev/null 2>/dev/null || true

run: 
	qemu-system-aarch64 -M raspi3 -kernel user2.img -display none -s -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb

pty: 
	qemu-system-aarch64 -M raspi3 -kernel user2.img -display none -s -serial null -serial pty -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb

