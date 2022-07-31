liburing_dir = /home/xcd/qemu_uintr/uring/liburing
CFLAGS += -static -O3
LDFLAGS += -I $(liburing_dir)/src/include  -L $(liburing_dir)/src/ -luring -D_GNU_SOURCE 
UINTR_FLAGS = -muintr -mgeneral-regs-only -minline-all-stringops



all:
	make normal
	make uintr
	make uring
	make gen


normal: utils.h normal.c
	gcc $(CFLAGS) normal.c -o normal $(LDFLAGS)


uring: uring.c utils.h
	gcc $(CFLAGS) uring.c -o uring $(LDFLAGS)
gen: utils.h
	gcc $(CFLAGS) data_gen.c -o gen $(LDFLAGS)

uintr: uintr.c utils.h
	gcc $(CFLAGS) $(UINTR_FLAGS) uintr.c -o uintr $(LDFLAGS)


move:
	cp *.txt ~/qemu_uintr/rootfs/
	cp normal ~/qemu_uintr/rootfs/
	cp uring  ~/qemu_uintr/rootfs/
	cp uintr ~/qemu_uintr/rootfs/