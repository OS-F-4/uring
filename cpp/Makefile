CXXFLAGS ?= -Wall -O2 -D_GNU_SOURCE  -std=c++2a -fcoroutines -static -muintr -mgeneral-regs-only -minline-all-stringops

all:
	g++ ${CXXFLAGS} echo_server.cpp -o echocpp -I ../liburing/src/include -L ../liburing/src/ -D_GNU_SOURCE -luring