
ROOT=/home/xcd/qemu_uintr/uring/go-cat
URING=/home/xcd/qemu_uintr/uring

all:
	gcc -c -o do_complete.o do_complete.c  -I $(URING)/liburing/src/include -L $(URING)/liburing/src/ -D_GNU_SOURCE  -luring
	ar rcs libdo_complete.a do_complete.o