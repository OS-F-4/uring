#include <stdio.h>
#include <liburing.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define COMP 7*100000
#define SMALL 4096
#define MID   4 * 1024 * 1024
#define LARGE 40*1024*1024
#define MAX_IO 1000
#define QUEUE_DEPTH 20

int io_num = 100;
int compute_num = 1000;

const char* small = "small.txt";
const char* mid = "mid.txt";
const char* large = "large.txt";
struct timeval start, totalstart, io_start;
struct timeval end, totalend, io_end;
int io_list[MAX_IO];
int comp_list[MAX_IO];
char io_res[MAX_IO];
unsigned compute_res[MAX_IO];
long io_delay[MAX_IO];


void prase_arg(int argc, char ** argv){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s io_num comp_num ...>\n",
                argv[0]);
        exit(1);
    }
    io_num = atoi(argv[1]);
    compute_num = atoi(argv[2]);
}



typedef struct user_data{
    char *buf;
    int fd;
} user_data;

void total_start(){
   gettimeofday(&totalstart, NULL);  
}

void total_end(){
   gettimeofday(&totalend, NULL);
}

void startio(){
    gettimeofday(&io_start, NULL);
}

void endio(int size, int index){
    gettimeofday(&io_end, NULL);
    io_delay[index] = (io_end.tv_sec - io_start.tv_sec)*1000000 +
    (io_end.tv_usec - io_start.tv_usec);
}


void start_clock(){
   gettimeofday(&start, NULL); 
}

void end_clock(char * s){
    gettimeofday(&end, NULL);
    printf("%s Using time : %ld us\n", s,(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
}


void out_put(){
    for(int i=0; i < io_num; i++){
        printf("%d io using %ld us\n", io_list[i], io_delay[i]);
    }
    printf("total Using time : %ld ms\n",
    (totalend.tv_sec-totalstart.tv_sec)*1000000+
    (totalend.tv_usec-totalstart.tv_usec)); 
}


unsigned compute(int size){
    unsigned long x = 2;
    for(unsigned long i = 1; i < COMP * size; i++){
         x *= i;
    }
    return (unsigned)x;
}


void help(int size){
    if(size < 0 || size > 2){
        printf("out of range!");
        return;
    }

    if(size == 0){
        FILE * out = fopen(small, "w");
        for(int i = 0; i < SMALL; ++i){
            int in = rand()%10;
            fprintf(out, "%d", in);
        }
        fclose(out);
    }else if(size == 1){
        FILE * out = fopen(mid, "w");
        for(int i = 0; i < MID; ++i){
            int in = rand()%10;
            fprintf(out, "%d", in);
        }
        fclose(out);
    }else{
        FILE * out = fopen(large, "w");
        for(int i = 0; i < LARGE; ++i){
            int in = rand()%10;
            fprintf(out, "%d", in);
        }
        fclose(out);
    }
}

void help_io_task(){
   FILE * out = fopen("io_task.txt", "w"); 
   if(!out){
    perror("fopen\n");
    exit(2);
   }
   for(int i =0; i < MAX_IO; i++){
        fprintf(out, "%d\n", rand()%3);
   }
   fclose(out);

    out = fopen("big_task.txt", "w"); 
   if(!out){
    perror("fopen\n");
    exit(2);
   }
   for(int i =0; i < MAX_IO; i++){
        fprintf(out, "%d\n", 2);
   }
   fclose(out);
}

void load_big_task(){
   FILE * in = fopen("big_task.txt", "r");
    if(!in){
        perror("fopen\n");
        exit(2);
    }
    for(int i=0; i < MAX_IO; i++){
        fscanf(in, "%d", &io_list[i]);
    }
    fclose(in); 
}

void load_io_task(){
    FILE * in = fopen("io_task.txt", "r");
    if(!in){
        perror("fopen\n");
        exit(2);
    }
    for(int i=0; i < MAX_IO; i++){
        fscanf(in, "%d", &io_list[i]);
    }
    fclose(in);
}

void help_comp_task(){
    FILE * out = fopen("comp_task.txt", "w");
    if(!out){
        perror("fopen\n");
        exit(2);
    }
    for(int i=0; i < MAX_IO; ++i){
        fprintf(out, "%d\n", 1 + rand()%10);
    }
    fclose(out);
}

void load_comp_task(){
    FILE * in = fopen("comp_task.txt", "r");
    if(!in){
        perror("fopen\n");
        exit(2);
    }
    for(int i=0; i < MAX_IO; ++i){
        fscanf(in, "%d", &comp_list[i]);
    }
    fclose(in);
}

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

void _read(int size){
    if(size < 0 || size > 2){
        printf("out of range!");
        return;
    }
    int file_fd, ret; 
    off_t sz;
    char * buf;
    if(size == 0){
        file_fd = open(small, O_RDONLY);
    }else if(size == 1){
        file_fd = open(mid, O_RDONLY);
    }else{
        file_fd = open(large, O_RDONLY);
    }
    if(file_fd < 0){
        perror("open error");
        exit(4);
    }
    sz = get_file_size(file_fd);
    buf = malloc(sz);
    ret = read(file_fd, buf, sz); 
    if(ret < 0){
        perror("read error\n");
        exit(2);
    }
    printf("%c\n", buf[3]);
    close(file_fd);
    free(buf);
}


user_data ud;

void submit_read(struct io_uring * ring, int size){
    if(size < 0 || size > 2){
        printf("out of range!");
        return;
    }
    int file_fd, ret; 
    off_t sz;
    char * buf;
    if(size == 0){
        file_fd = open(small, O_RDONLY);
    }else if(size == 1){
        file_fd = open(mid, O_RDONLY);
    }else{
        file_fd = open(large, O_RDONLY);
    }
    if(file_fd < 0){
        perror("open error");
        exit(4);
    }
    sz = get_file_size(file_fd);
    buf = malloc(sz);
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_read(sqe, file_fd, buf, sz, 0);
    ud.buf = buf;
    ud.fd = file_fd;
    io_uring_sqe_set_data(sqe, &ud);
    io_uring_submit(ring); 
}

void complete(struct io_uring * ring, struct io_uring_cqe * cqe){
    if (cqe->res < 0) {
        fprintf(stderr, "Async readv failed.\n");
        exit(1);
    }
    user_data * data = io_uring_cqe_get_data(cqe);
    printf("%c\n", data->buf[3]);
    free(data->buf);
    close(data->fd);
    io_uring_cqe_seen(ring, cqe);
}

void peek_and_next(struct io_uring *ring){


}


