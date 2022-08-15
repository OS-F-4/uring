#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>
#include <stdlib.h>
#include <syscall.h>
#include <unistd.h>
#include <pthread.h>
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

#define QUEUE_DEPTH 20
#define BLOCK_SZ    4096
struct io_uring ring;
int completed = 0;


struct file_info {
    off_t file_sz;
    struct iovec iovecs[];      /* Referred by readv/writev */
};

/*
* Returns the size of the file whose open file descriptor is passed in.
* Properly handles regular file and block devices as well. Pretty.
* */

off_t get_file_size(int fd) {
    struct stat st;

    if(fstat(fd, &st) < 0) {
        perror("fstat");
        return -1;
    }
    if (S_ISBLK(st.st_mode)) {
        unsigned long long bytes;
        if (ioctl(fd, BLKGETSIZE64, &bytes) != 0) {
            perror("ioctl");
            return -1;
        }
        return bytes;
    } else if (S_ISREG(st.st_mode))
        return st.st_size;

    return -1;
}

/*
 * Output a string of characters of len length to stdout.
 * We use buffered output here to be efficient,
 * since we need to output character-by-character.
 * */
void output_to_console(char *buf, int len) {
    while (len--) {
        fputc(*buf++, stdout);
    }
}

/*
 * Wait for a completion to be available, fetch the data from
 * the readv operation and print it to the console.
 * */

int get_completion_and_print(struct io_uring *ring) {
    struct io_uring_cqe *cqe;
    int ret = io_uring_wait_cqe(ring, &cqe);
    if (ret < 0) {
        perror("io_uring_wait_cqe");
        return 1;
    }
    if (cqe->res < 0) {
        fprintf(stderr, "Async readv failed.\n");
        return 1;
    }
    struct file_info *fi = io_uring_cqe_get_data(cqe);
    int blocks = (int) fi->file_sz / BLOCK_SZ;
    if (fi->file_sz % BLOCK_SZ) blocks++;
    for (int i = 0; i < blocks; i ++)
        output_to_console(fi->iovecs[i].iov_base, fi->iovecs[i].iov_len);

    io_uring_cqe_seen(ring, cqe);
    return 0;
}

int while_completion_and_print(struct io_uring *ring) {
    struct io_uring_cqe *cqe;
    do{
    int ret = io_uring_peek_cqe(ring, &cqe);
    if (ret < 0) {
        perror("io_uring_wait_cqe");
        return 1;
    }
    if (cqe->res < 0) {
        fprintf(stderr, "Async readv failed.\n");
        return 1;
    }
    struct file_info *fi = io_uring_cqe_get_data(cqe);
    int blocks = (int) fi->file_sz / BLOCK_SZ;
    if (fi->file_sz % BLOCK_SZ) blocks++;
    // for (int i = 0; i < blocks; i ++)
    //     output_to_console(fi->iovecs[i].iov_base, fi->iovecs[i].iov_len);

    io_uring_cqe_seen(ring, cqe);
    completed++;
    printf("- - - - complete:%d\n", completed);
    }while(cqe);
    return 0;
}

/*
 * Submit the readv request via liburing
 * */

int submit_read_request(char *file_path, struct io_uring *ring) {
    int file_fd = open(file_path, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return 1;
    }
    off_t file_sz = get_file_size(file_fd);
    off_t bytes_remaining = file_sz;
    off_t offset = 0;
    int current_block = 0;
    int blocks = (int) file_sz / BLOCK_SZ;
    if (file_sz % BLOCK_SZ) blocks++;
    struct file_info *fi = malloc(sizeof(*fi) +
                                          (sizeof(struct iovec) * blocks));

    /*
     * For each block of the file we need to read, we allocate an iovec struct
     * which is indexed into the iovecs array. This array is passed in as part
     * of the submission. If you don't understand this, then you need to look
     * up how the readv() and writev() system calls work.
     * */
    while (bytes_remaining) {
        off_t bytes_to_read = bytes_remaining;
        if (bytes_to_read > BLOCK_SZ)
            bytes_to_read = BLOCK_SZ;

        offset += bytes_to_read;
        fi->iovecs[current_block].iov_len = bytes_to_read;

        void *buf;
        if( posix_memalign(&buf, BLOCK_SZ, BLOCK_SZ)) {
            perror("posix_memalign");
            return 1;
        }
        fi->iovecs[current_block].iov_base = buf;

        current_block++;
        bytes_remaining -= bytes_to_read;
    }
    fi->file_sz = file_sz;

    /* Get an SQE */
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    /* Setup a readv operation */
    io_uring_prep_readv(sqe, file_fd, fi->iovecs, blocks, 0);
    /* Set user data */
    io_uring_sqe_set_data(sqe, fi);
    /* Finally, submit the request */
    io_uring_submit(ring);

    return 0;
}
pthread_mutex_t awake_lock;
bool awake=false;
pthread_t get_completion_thread;

void thread(void){
    pthread_detach(pthread_self());
    while_completion_and_print(&ring);
    printf("- - - - leave\n");
    pthread_mutex_lock(&awake_lock);
    awake = false;
    pthread_mutex_unlock(&awake_lock);
  
    pthread_exit(NULL);
}

void __attribute__ ((interrupt)) uintr_handler(struct __uintr_frame *ui_frame,
					       unsigned long long vector)
{
    // get_completion_and_print(&ring);
    pthread_mutex_lock(&awake_lock);
    if(awake){
        printf("awake return\n");
    }else{
        awake = true;
        printf("~ ~ ~ ~init\n");
        pthread_create(&get_completion_thread, 0, (void *)thread, 0);
    }
    pthread_mutex_unlock(&awake_lock); 
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [file name] <[file name] ...>\n",
                argv[0]);
        return 1;
    }
    
    if (uintr_register_handler(uintr_handler, 0)) {
		printf("Interrupt handler register error\n");
		exit(EXIT_FAILURE);
	}

    int ret = uintr_create_fd(0, 0);
    if(! ret){
        printf("error when creat fd\n");
        exit(2);
    }
    _stui();
    int uintr_fd = ret;
    // int uipi_index = uintr_register_sender(uintr_fd, 0);

    /* Initialize io_uring */
    struct io_uring_params par;
    memset(&par, 0, sizeof(struct io_uring_params));
    #ifndef kernel518 
    par.uintr_fd = uintr_fd;
    #endif

    par.flags |= IORING_SETUP_SQPOLL;
    io_uring_queue_init_params(QUEUE_DEPTH, &ring, &par);

    #ifdef kernel518
    io_uring_register_uintr(&ring, &uintr_fd);
    #endif
    
    int need = 0;
    for (int i = 1; i < argc; i++) {
        int ret = submit_read_request(argv[i], &ring);
        need ++;
        printf("sleeping\n");
        sleep(0.05);
        if (ret) {
            fprintf(stderr, "Error reading file: %s\n", argv[i]);
            return 1;
        }
    }
    unsigned long n  = 1;
    while (completed < need && n <10000000){
        n++;
        if(n % 100000 == 0){
            printf("do compute\n");
        }
    }
    /* Call the clean-up function. */
    io_uring_queue_exit(&ring);
    _clui();
    printf("exit\n");
    return 0;
}
