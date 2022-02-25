SFILES = $(wildcard src/*.c)
OFILES = $(patsubst %.c, %.o, $(SFILES))
# OFILES = $(SFILES:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img

src/start.o: src/start.S
	aarch64-linux-gnu-gcc -c src/start.S -o src/start.o

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

kernel8.img: src/start.o $(OFILES)
	aarch64-linux-gnu-ld -nostdlib -nostartfiles src/start.o $(OFILES) -T src/linker.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm *.elf src/*.o *.img >/dev/null 2>/dev/null || true

run: 
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
