#include <stdio.h>
#include <stdlib.h>

void swap(int* a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;	
}
int A;                    //全局未初始化变量
int B = 0;                //全局初始化为0的变量
int C = 2;                //全局初始化变量
int static D;                //全局静态未初始化变量
int static E = 0;            //全局静态初始化为0的变量
int static F = 4;            //全局静态初始化变量
int const G = 5;            //全局常量

int main()
{

    int a;                //局部未初始化变量
    int b = 0;                //局部初始化为0的变量
    int c = 2;                //局部初始化变量
    int static d;            //局部静态未初始化变量
    int static e = 0;            //局部静态初始化为0的变量
    int static f = 4;            //局部静态初始化变量
    int const g = 5;            //局部常量
    swap(&b,&c);
    char h1[] = "abcde";        //局部字符数组
    char* h2 = "abcde";            //局部指针指向字符串常量

    int* i = malloc(sizeof(int));    //堆
	printf("code addr = %p\n", main);
    printf("A_global_addr = %p\n",&A);
    printf("B_global_init0_addr = %p\n",&B);
    printf("C_global_init_addr = %p\n",&C);
    printf("D_global_static_addr = %p\n",&D);
    printf("E_global_static_init0_addr = %p\n",&E);
    printf("F_global_static_init_addr = %p\n",&F);
    printf("G_global_const_addr = %p\n",&G);

    printf("\n");

    printf("a_addr = %p\n",&a);
    printf("b_init0_addr = %p\n",&b);
    printf("c_init_addr = %p\n",&c);
    printf("d_static_addr = %p\n",&d);
    printf("e_static_init0_addr = %p\n",&e);
    printf("f_static_init_addr = %p\n",&f);
    printf("g_const_addr = %p\n",&g);

    printf("\n");

    printf("h1_arr_addr = %p\n",h1);
    printf("h2_strconst_addr = %p\n",h2);
    printf("h2_point_addr = %p\n",&h2);
    printf("i_malloc_addr = %p\n",i);

    while(1);

    free(i);

    return 0;
}
