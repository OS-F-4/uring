liburing_dir = /home/xcd/qemu_uintr/uring/liburing
CFLAGS +=  -O2  -Wall  -static -muintr -mgeneral-regs-only -minline-all-stringops
all:
	gcc $(CFLAGS) main.c -o web -I $(liburing_dir)/src/include  -L $(liburing_dir)/src/ -D_GNU_SOURCE  -luring -lpthread

move:
	cp ./web /home/xcd/qemu_uintr/initramfs/x86_64_busybox
	cp ./runweb.sh /home/xcd/qemu_uintr/initramfs/x86_64_busybox
	cp ./test.txt /home/xcd/qemu_uintr/initramfs/x86_64_busybox
	cp ./web /home/xcd/qemu_uintr/rootfs
	cp ./runweb.sh /home/xcd/qemu_uintr/rootfs
	cp ./test.txt /home/xcd/qemu_uintr/rootfs 
	cd /home/xcd/qemu_uintr/rootfs && ./pack.sh