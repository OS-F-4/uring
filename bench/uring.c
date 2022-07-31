#include "utils.h"
#include <string.h>

int main(int argc, char ** argv){
    prase_arg(argc, argv);
    load_io_task();
    load_comp_task();
    int io_index, comp_index;
    io_index = comp_index = 0;
    struct io_uring ring;
    struct io_uring_params par;
    struct io_uring_cqe *cqe = NULL;

    memset(&par, 0, sizeof(struct io_uring_params));
    par.flags |= IORING_SETUP_SQPOLL;
    io_uring_queue_init_params(QUEUE_DEPTH, &ring, &par);
    total_start();

    while(io_index<io_num || comp_index < compute_num){
        if(io_index<io_num){
            startio();
            submit_read(&ring, io_list[io_index]);
        }
    do_comp:
        if(comp_index < compute_num)
            for(int j=0; j <1 ;j++) {
                compute_res[comp_index] = compute(comp_list[comp_index]);
                comp_index ++;
            }
        
        if(io_index < io_num){
            int ret = io_uring_peek_cqe(&ring, &cqe);
            if(!cqe)goto do_comp;
            else{
                complete(&ring, cqe);
                endio(io_list[io_index], io_index);
                io_index ++;
            }
        }
    }
    total_end();
    out_put();
    return 0;
}