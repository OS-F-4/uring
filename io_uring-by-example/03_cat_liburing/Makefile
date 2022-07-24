
CFLAGS +=  -O2  -Wall  -static -muintr -mgeneral-regs-only -minline-all-stringops
ex += -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone
all:
	gcc $(CFLAGS) main.c -o caturing  -I ../../liburing/src/include -L ../../liburing/src/ -D_GNU_SOURCE  -luring -lpthread

move:
	cp ./caturing /home/xcd/qemu_uintr/initramfs/x86_64_busybox
