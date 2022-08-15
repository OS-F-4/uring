#include "utils.h"
#include <string.h>
#include <syscall.h>
#include <unistd.h>
#include <x86gprintrin.h>

#ifdef kernel518
#define __NR_uintr_register_handler	471
#define __NR_uintr_unregister_handler	472
#define __NR_uintr_create_fd		473
#define __NR_uintr_register_sender	474
#define __NR_uintr_unregister_sender	475
#else
#define __NR_uintr_register_handler	449
#define __NR_uintr_unregister_handler	450
#define __NR_uintr_create_fd		451
#define __NR_uintr_register_sender	452
#define __NR_uintr_unregister_sender	453
#endif

/* For simiplicity, until glibc support is added */
#define uintr_register_handler(handler, flags)	syscall(__NR_uintr_register_handler, handler, flags)
#define uintr_unregister_handler(flags)		syscall(__NR_uintr_unregister_handler, flags)
#define uintr_create_fd(vector, flags)		syscall(__NR_uintr_create_fd, vector, flags)
#define uintr_register_sender(fd, flags)	syscall(__NR_uintr_register_sender, fd, flags)
#define uintr_unregister_sender(fd, flags)	syscall(__NR_uintr_unregister_sender, fd, flags)

struct io_uring ring;
struct io_uring_cqe *cqe = NULL;
int io_index, comp_index;
int uintr_fd;

void __attribute__ ((interrupt)) uintr_handler(struct __uintr_frame *ui_frame,
					       unsigned long long vector)
{   
    printf("in handler\n");
}




void setup_uintr(struct io_uring * ring){
    int ret;
    if (ret = uintr_register_handler(uintr_handler, 0)) {
		printf("Interrupt handler register error %d\n", ret);
		exit(EXIT_FAILURE);
	}

    uintr_fd = uintr_create_fd(0, 0);
    if(! uintr_fd){
        printf("error when creat fd\n");
        exit(2);
    }
    #ifdef kernel518
    io_uring_register_uintr(ring, &uintr_fd);
    #endif
    _stui();
}

int main(int argc, char ** argv){
    prase_arg(argc, argv);
    load_io_task();
    load_comp_task();
    io_index = comp_index = 0;
    struct io_uring_params par;
    memset(&par, 0, sizeof(struct io_uring_params));
    
    #ifndef kernel518
    par.uintr_fd = uintr_fd;
    #endif

    par.flags |= IORING_SETUP_SQPOLL;
    io_uring_queue_init_params(QUEUE_DEPTH, &ring, &par);
    setup_uintr(&ring);

    total_start();

    startio();
    submit_read(&ring, 1);


    compute(1000); 
    _clui();
    total_end();
    out_put();
    return 0;
}
