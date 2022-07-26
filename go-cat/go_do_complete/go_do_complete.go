package main


//#cgo CFLAGS: -I./do_complete -I /home/xcd/qemu_uintr/uring/liburing/src/include
//#cgo LDFLAGS: -L${SRCDIR}/do_complete -ldo_complete  -L /home/xcd/qemu_uintr/uring/liburing/src/  -luring
//#include "do_complete.h"
import "C"

import "fmt"

import "C"

//export go_add
func go_add() {
	for i :=0; i<5; i ++{
		go C.add(C.int(i))
	}
	fmt.Println("ok")
}

//export go_do_complete
func go_do_complete(cqe  *C.struct_io_uring_cqe ){
	go C.do_complete_cqe(cqe)
}




func main(){}