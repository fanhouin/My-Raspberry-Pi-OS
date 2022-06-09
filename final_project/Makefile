all: clean build run

build:
	gcc -Wall ssd_fuse.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o ssd_fuse.o
	gcc -Wall ssd_fuse_dut.c -o ssd_fuse_dut.o

clean:
	rm *.o >/dev/null 2>/dev/null || true

run:
	./ssd_fuse.o -d /tmp/ssd

test:
	sh test.sh test1