
CFLAGS +=  -O2  -Wall  -static -muintr -mgeneral-regs-only -minline-all-stringops
ex += -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone
all:
	gcc $(CFLAGS) main.c ./selfwrite/go_do_search.a -o couring  -I ../liburing/src/include -L ../liburing/src/ -D_GNU_SOURCE  -luring -lpthread -I ./selfwrite  -L./selfwrite 

move:
	cp ./couring /home/xcd/qemu_uintr/rootfs
	cd /home/xcd/qemu_uintr/rootfs && ./pack.sh
	cd -
	run
