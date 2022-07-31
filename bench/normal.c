#include "utils.h"
int main(int argc, char ** argv){
    prase_arg(argc, argv);
    load_io_task();
    load_comp_task();
    total_start();
    for(int i=0; i < io_num; i++){
        startio();
        _read(io_list[i]);
        endio(io_list[i], i);
    }
    start_clock();
    for(int i=0; i< compute_num; i++){
        compute_res[i] = compute(comp_list[i]);
    }
    end_clock("compute ");
    total_end();
    out_put();
    return 0;
}