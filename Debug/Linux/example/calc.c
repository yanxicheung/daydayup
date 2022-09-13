#include "calc.h"
#include <string.h>
#include <stdlib.h>
int add(int a, int b)
{
	return a + b;
}

int sub(int a, int b)
{
	return a - b;
}

int multi(int a, int b)
{
	return a * b;
}

int divide(int a, int b)
{
	return a / b;
}

char *strcpy_(char* dest, const char *src)
{
	strcpy(dest, src);
}

void double_free(char* p)
{
	for(int i = 0; i< 10;++i)
	{
		free(p);
	}
}

int segmentfault()
{
	printf("segmentfault addr %p\n", segmentfault);
	int num = 10;
	int ret = 0x00;  
    int *pTemp = NULL;  
      
    *pTemp = 0x01;  /* 这将导致一个段错误，致使程序崩溃退出 */  
      
    ret = num + *pTemp;  
      
    return ret;  
}
