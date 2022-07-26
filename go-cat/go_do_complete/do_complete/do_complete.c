
#include "do_complete.h"

void do_complete_cqe(struct io_uring_cqe *cqe){
    struct file_info *fi = io_uring_cqe_get_data(cqe);
    int blocks = (int) fi->file_sz / BLOCK_SZ;
    if (fi->file_sz % BLOCK_SZ) blocks++;
    printf("completing an cqe\n");
    sleep(0.5);
    // for (int i = 0; i < blocks; i ++)
    //     output_to_console(fi->iovecs[i].iov_base, fi->iovecs[i].iov_len);
}

void add(int a){
    printf("%d\n",a);
    sleep(1);
    printf("finish\n");
}