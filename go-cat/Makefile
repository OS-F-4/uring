
CFLAGS +=  -O2  -Wall  -static -muintr -mgeneral-regs-only -minline-all-stringops
ex += -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone
ROOTFS = /home/xcd/qemu_uintr/rootfs
all:
	gcc $(CFLAGS) main.c ./selfwrite/go_do_search.a -o couring  -I ../liburing/src/include -L ../liburing/src/ -D_GNU_SOURCE  -luring -lpthread -I ./selfwrite  -L./selfwrite 

move:
	cp ./couring $(ROOTFS)
	# pack the rootfs
	cd $(ROOTFS) && find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../ubuntu-x86_64.cpio.gz
	cd -
	run
