liburing_dir = ~/qemu_uintr/uring/liburing
CFLAGS += -static -O3
LDFLAGS += -I $(liburing_dir)/src/include  -L $(liburing_dir)/src/ -luring -D_GNU_SOURCE 
UINTR_FLAGS = -muintr -mgeneral-regs-only -minline-all-stringops
ROOTFS=~/qemu_uintr/rootfs/

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

uintr18: uintr.c utils.h
	gcc $(CFLAGS) $(UINTR_FLAGS) uintr.c -o uintr $(LDFLAGS) -Dkernel518



move:
	cp *.txt $(ROOTFS)
	cp normal  $(ROOTFS)
	cp uring   $(ROOTFS)
	cp uintr  $(ROOTFS)
