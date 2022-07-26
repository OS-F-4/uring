package main


//#cgo CFLAGS: -I./do_search -I /home/xcd/qemu_uintr/uring/liburing/src/include -I ../go_do_complete/do_complete -I ../go_do_complete
//#cgo LDFLAGS: -L${SRCDIR}/do_search -ldo_search  -L /home/xcd/qemu_uintr/uring/liburing/src/  -luring  -L ../go_do_complete -lgo_do_complete -L ../go_do_complete/do_complete -ldo_complete
//#include "do_search.h"
import "C"


import "C"



//export go_do_search
func go_do_search(ring  *C.struct_io_uring ){
	go C.do_search_cqe(ring)
}




func main(){}