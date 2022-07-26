#include <liburing.h>
#include <unistd.h>
#include "/home/xcd/qemu_uintr/uring/go-cat/utils.h"


void do_complete_cqe(struct io_uring_cqe *cqe);
