liburing_dir = /home/xcd/qemu_uintr/uring/liburing

all:
	gcc -static main.c -o web -I $(liburing_dir)/src/include  -L $(liburing_dir)/src/ -D_GNU_SOURCE  -luring
