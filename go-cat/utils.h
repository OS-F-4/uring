
#include <stdio.h>
#include <sys/ioctl.h>
#define BLOCK_SZ    4096



struct file_info {
    off_t file_sz;
    struct iovec iovecs[];      /* Referred by readv/writev */
};