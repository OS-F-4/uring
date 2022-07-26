
#include "do_search.h"

int do_search_cqe(struct io_uring *ring) {
    struct io_uring_cqe *cqe;
    do{
    int ret = io_uring_peek_cqe(ring, &cqe);
    if (ret < 0) {
        perror("io_uring_peek_cqe");
        return 1;
    }
    if (cqe->res < 0) {
        fprintf(stderr, "Async readv failed.\n");
        return 1;
    }
    // go_do_complete_cqe
    go_do_complete(cqe);
    
    io_uring_cqe_seen(ring, cqe);
    completed++;
    printf("- - - - complete:%d\n", completed);
    }while(cqe);
    pthread_mutex_lock(&awake_lock);
    awake = false;
    pthread_mutex_unlock(&awake_lock);

    return 0;
}