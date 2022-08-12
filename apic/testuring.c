#include "utils.h"
#include <string.h>
#include <syscall.h>
#include <unistd.h>
#include <x86gprintrin.h>
#define __NR_uintr_register_handler	449
#define __NR_uintr_unregister_handler	450
#define __NR_uintr_create_fd		451
#define __NR_uintr_register_sender	452
#define __NR_uintr_unregister_sender	453

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
    // int ret = io_uring_peek_cqe(&ring, &cqe);
    // if(ret < 0) printf("in handler but no cqe\n");
    // if(!cqe) return;
    // complete(&ring, cqe);
    // endio(io_list[io_index], io_index);
    // io_index ++;
    // if(io_index< io_num){
    //     // printf("submit %d\n", io_index);
    //     startio();
    //     submit_read(&ring, io_list[io_index]);
    // }
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
    io_uring_register_uintr(ring, &uintr_fd);
    _stui();
}

int main(int argc, char ** argv){
    prase_arg(argc, argv);
    load_io_task();
    load_comp_task();
    io_index = comp_index = 0;
    struct io_uring_params par;
    memset(&par, 0, sizeof(struct io_uring_params));
    // par.uintr_fd = uintr_fd;
    // par.flags |= IORING_SETUP_SQPOLL;
    io_uring_queue_init_params(QUEUE_DEPTH, &ring, &par);
    setup_uintr(&ring);

    total_start();

    startio();
    submit_read(&ring, io_list[io_index]);

    // while(comp_index < compute_num){
    //     compute_res[comp_index] = compute(comp_list[comp_index]);
    //     comp_index ++;
    // }


    // while (io_index < io_num){
    //     compute(10);
    // }
    compute(1000); 
    _clui();
    total_end();
    out_put();
    return 0;
}