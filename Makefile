CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
INCLUDE = -Iinclude

SFILES = $(wildcard src/*.c)
OFILES = $(patsubst %.c, %.o, $(SFILES)) # OFILES = $(SFILES:.c=.o)
BOOTFILES = $(wildcard boot/*.S)
BOOTOFILES = $(patsubst %.S, %.o, $(BOOTFILES)) # BOOTOFILES = $(BOOTFILES:.S=.o)
ALLOFILES = $(OFILES) $(BOOTOFILES)

CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img run

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ 

%.o: %.c
	$(CC) $(CFLAGS)	$(INCLUDE) -c $< -o $@

kernel8.img: $(ALLOFILES)
	$(LD) -nostdlib -nostartfiles  $(ALLOFILES) -T boot/linker.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm *.elf *.img src/*.o boot/*.o >/dev/null 2>/dev/null || true

run: 
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -s -serial null -serial stdio
