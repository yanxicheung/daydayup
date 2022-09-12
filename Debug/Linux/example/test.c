#include <stdio.h>
#include "calc.h"
int main(int argc, char **argv)
{
	printf("hello world\n");
	printf("add(1,2)=%d\n", add(1,2));
	printf("sub(1,2)=%d\n", sub(1,2));
	printf("multi(1,2)=%d\n", multi(1,2));
	printf("divide(2,1)=%d\n", divide(2,1));
	//divide(2,0);
	while(1);
	return 0;
}
