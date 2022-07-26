package main
import (
	"fmt"
    "os"
    "sync"
    // "unsafe"
)
/*
#cgo CFLAGS: -I /home/xcd/qemu_uintr/uring/liburing/src/include 
#cgo LDFLAGS: -L/home/xcd/qemu_uintr/uring/liburing/src/ -luring -lpthread 
#include <liburing.h>
#include <pthread.h>
#include "do_complete.h"
static pthread_mutex_t awake_lock;



*/
import "C"


var searched int = 0

//export  awake
var awake bool = false;

//export get_searched
func get_searched()int {
    return searched
}

var lck sync.Mutex

//export switch_awake
func switch_awake(b bool){
    awake = b
}


//export lock_awake
func lock_awake(){
    lck.Lock()
}

//export unlock_awake
func unlock_awake(){
    lck.Unlock()
}

//export is_awake
func is_awake()bool{
    return awake
}


func do_search(ring *C.struct_io_uring){
	var cqe * C.struct_io_uring_cqe
	for{
		var ret = C.io_uring_peek_cqe(ring, &cqe)
        if cqe == nil {
            fmt.Println("empty pointer")
            break
        }
        if ret < 0 {
            fmt.Println("peek error")
            os.Exit(1)
        }
        if cqe.res < 0{
            fmt.Println("res error")
            os.Exit(2) 
        }
        go C.do_complete_cqe(cqe)
        searched++
        fmt.Println("searched:", searched)
        C.io_uring_cqe_seen(ring, cqe)
	}

    lock_awake()
    awake = false
    unlock_awake()
}

//export go_do_search
func go_do_search(ring *C.struct_io_uring){
    go do_search(ring)
    fmt.Println("after go search")
}









func main(){
	
	fmt.Println("ok")
}