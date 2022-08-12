liburing_dir = /home/xcd/qemu_uintr/uring/liburing
CFLAGS += -static -O3
LDFLAGS += -I $(liburing_dir)/src/include  -L $(liburing_dir)/src/ -luring -D_GNU_SOURCE 
UINTR_FLAGS = -muintr -mgeneral-regs-only -minline-all-stringops
ROOTFS=~/qemu_uintr/rootfs/

all:
	make apic
	make gen

apic:
	gcc $(CFLAGS) $(UINTR_FLAGS) testuring.c -o apictest $(LDFLAGS) 

gen: utils.h
	gcc $(CFLAGS) data_gen.c -o gen $(LDFLAGS)

move:
	cp *.txt $(ROOTFS)
	cp apictest $(ROOTFS)
