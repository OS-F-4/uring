
package main

//#cgo CFLAGS: -I./number
//#cgo LDFLAGS: -L${SRCDIR}/number -lnumber
//#include "number.h"
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

func main(){
	//go_add()
	//fmt.Println( 3)
	//C.add(C.int(1))
}



