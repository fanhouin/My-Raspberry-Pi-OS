all: clean build run

build:
	gcc -Wall ssd_fuse.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o ssd_fuse
	gcc -Wall ssd_fuse_dut.c -o ssd_fuse_dut

clean:
	rm *.o >/dev/null 2>/dev/null || true

run:
	./ssd_fuse -d /tmp/ssd

test:
	bash test.sh test1
	bash test.sh test2
	bash godtest.sh test1
	bash godtest.sh test2
	bash godtest.sh test3
	bash godtest.sh test4