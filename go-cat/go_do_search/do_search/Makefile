ROOT=/home/xcd/qemu_uintr/uring/go-cat
URING=/home/xcd/qemu_uintr/uring
COMP=$(URING)/go-cat/go_do_complete/
IFLAGS=-I $(URING)/liburing/src/include  -I $(COMP) -I $(COMP)/do_complete
LFLAGS=-L $(URING)/liburing/src  -luring -L $(COMP)  -L $(COMP)/do_complete
all:
	gcc -c -o do_search.o do_search.c $(URING)/go-cat/go_do_complete/go_do_complete.a $(IFLAGS) . $(LFLAGS) -D_GNU_SOURCE  
	ar rcs libdo_search.a do_search.o