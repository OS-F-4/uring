#include "number.h"
#include <stdio.h>
#include <unistd.h>
int add(int mod){
	sleep(1);
	printf("C add function %d\n", mod);
	return mod;
}
