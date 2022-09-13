# 背景

最近项目中用到了一个库，在程序崩溃时可以生成`exception`文件，记录程序崩溃时的调用信息，对于定位问题比较有价值，因此整理下这个库涉及到的知识点。相关测试代码已经放到`github`可以下载调试。



# 基础知识

## maps

`maps`用来描述进程的虚拟地址空间是如何使用的。总共包括六列，每列及其含义如下：

| 名字     | 含义                                                         |
| -------- | ------------------------------------------------------------ |
| address  | 本段在虚拟内存中的地址范围。                                 |
| perms    | 本段的权限，r-读，w-写，x-执行， p-私有，s-共享。            |
| offset   | 即本段映射地址在文件中的偏移。                               |
| dev      | 主设备号与次设备号：所映射的文件所属设备的设备号。           |
| inode    | 文件索引节点号。                                             |
| pathname | 映射的文件名。<br />对有名映射而言，是映射的文件名。<br />对匿名映射来说，是此段内存在进程中的作用。<br />[stack]表示本段内存作为栈来使用，[heap]作为堆来使用，其他情况则为无。 |

对于有名的内存区间而言，属性为`r--p`表示存放的是`rodata`;属性为`rw-p`存放的是`bss`和`data`;属性为`r-xp`表示存放的是`text`数据。没有文件名的内存区间则表示用`mmap`映射的匿名空间。

以下为`./example/maps_test.c`编译成的可执行文件`mapstest`的运行结果：

```shell
code addr = 0x55e1df08d6da
A_global_addr = 0x55e1df28e034
B_global_init0_addr = 0x55e1df28e020
C_global_init_addr = 0x55e1df28e010
D_global_static_addr = 0x55e1df28e024
E_global_static_init0_addr = 0x55e1df28e028
F_global_static_init_addr = 0x55e1df28e014
G_global_const_addr = 0x55e1df08d998

a_addr = 0x7ffce299bb90
b_init0_addr = 0x7ffce299bb94
c_init_addr = 0x7ffce299bb98
d_static_addr = 0x55e1df28e02c
e_static_init0_addr = 0x55e1df28e030
f_static_init_addr = 0x55e1df28e018
g_const_addr = 0x7ffce299bb9c

h1_arr_addr = 0x7ffce299bbb2
h2_strconst_addr = 0x55e1df08d99c
h2_point_addr = 0x7ffce299bba0
i_malloc_addr = 0x55e1dfd1d260
```

`ps -ef| grep mapstest`得到进程对应的`pid`号，maps文件如下：（路径为`/proc/{pid}/maps`）

```shell
55e1df08d000-55e1df08e000 r-xp 00000000 08:01 24786338  /mapstest    #text
55e1df28d000-55e1df28e000 r--p 00000000 08:01 24786338  /mapstest    #rodata
55e1df28e000-55e1df28f000 rw-p 00001000 08:01 24786338  /mapstest    #bss data
55e1dfd1d000-55e1dfd3e000 rw-p 00000000 00:00 0         [heap]       #堆
7f64881e5000-7f64883cc000 r-xp 00000000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f64883cc000-7f64885cc000 ---p 001e7000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f64885cc000-7f64885d0000 r--p 001e7000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f64885d0000-7f64885d2000 rw-p 001eb000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f64885d2000-7f64885d6000 rw-p 00000000 00:00 0 
7f64885d6000-7f64885fd000 r-xp 00000000 08:01 10490421  /lib/x86_64-linux-gnu/ld-2.27.so
7f64887de000-7f64887e0000 rw-p 00000000 00:00 0 
7f64887fd000-7f64887fe000 r--p 00027000 08:01 10490421  /lib/x86_64-linux-gnu/ld-2.27.so
7f64887fe000-7f64887ff000 rw-p 00028000 08:01 10490421  /lib/x86_64-linux-gnu/ld-2.27.so
7f64887ff000-7f6488800000 rw-p 00000000 00:00 0 
7ffce297d000-7ffce299e000 rw-p 00000000 00:00 0          [stack]    #栈
7ffce29ef000-7ffce29f2000 r--p 00000000 00:00 0          [vvar]
7ffce29f2000-7ffce29f4000 r-xp 00000000 00:00 0          [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0  [vsyscall]
```

对应地，我们可以找到每个变量在虚拟内存中的地址范围。其中**动态链接库是程序运行时动态加载的而其加载地址也是每次可能不一样的**。



## 信号

​        `Linux`中的信号是一种消息处理机制, 它本质上是一个整数，不同的信号对应不同的值，由于信号的结构简单所以天生不能携带很大的信息量，但是信号在系统中的优先级是非常高的。

​        在`Linux`中的很多常规操作中都会有相关的信号产生，先从我们最熟悉的场景说起：

​        `通过键盘操作产生了信号`：用户按下`Ctrl-C`，这个键盘输入产生一个硬件中断，使用这个快捷键会产生信号, 这个信号会杀死对应的某个进程。

​       `通过shell命令产生了信号`：通过`kill`命令终止某一个进程，`kill  -9  进程PID`。
`通过函数调用产生了信号`：如果CPU当前正在执行这个进程的代码调用，比如函数 sleep()，进程收到相关的信号，被迫挂起。

​       `通过对硬件进行非法访问产生了信号`：正在运行的程序访问了非法内存，发生段错误，进程退出。

​        信号也可以实现进程间通信，但是信号能传递的数据量很少，不能满足大部分需求，另外信号的优先级很高，并且它对应的处理动作是回调完成的，它会打乱程序原有的处理流程，影响到最终的处理结果。因此非常不建议使用信号进行进程间通信。

​        通过 `kill -l` 命令可以查看系统定义的信号列表。

进程对信号的处理可以有以下三种措施：

1. 忽略这个信号；

2. 执行用户定义相应操作；

3. 执行默认的操作；

`SIGKILL`和`SIGTSTOP`是不可以被信号处理函数捕捉或者忽略。

常用接口：

| 函数名      | 备注                                            |
| ----------- | ----------------------------------------------- |
| `signal`    | 信号安装函数，但是有消息重入问题，不建议使用    |
| `sigaction` | 信号安装函数                                    |
| `kill`      | 给指定进程发送信号                              |
| `raise`     | 给自己发送信号，和`kill(getpid( ),sig)`等价     |
| `alarm`     | 设置定时器，定时器超时后，发送一个`SIGALRM`信号 |

**例子：**

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void catch_signal(int sig)
 {
    switch(sig)
    {
        case SIGINT:printf("get SIGINT signal\n");    
    }

}

int main (int argc,char *argv[ ])
{
    signal(SIGINT,catch_signal);
    int i = 0;
    while(1)
    {
        sleep(100);//执行完信号后sleep（）立即返回，不会一直休眠下去
        printf("hello i = %d",i++);
    }
    return 0;
}
```

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int temp = 0;

void handler_sigint(int signo)
{
	printf("recv SIGINT\n");
	sleep(5);
	temp += 1;
	printf("the value of temp is:%d\n",temp);
	printf("in handler_sigint, after sleep\n");
}

int main()
{
	struct sigaction act;
	act.sa_handler = handler_sigint;
	act.sa_flags = SA_NOMASK;
	sigaction(SIGINT, &act, NULL);
	while(1);
	return 0;
}
```



## objdump

`objdump -d`可以将目标文件、动态库、可执行文件反汇编，如下：

```asm
00000000000006da <swap>:
 6da:	55                   	push   %rbp
 6db:	48 89 e5             	mov    %rsp,%rbp
 6de:	48 89 7d e8          	mov    %rdi,-0x18(%rbp)
 6e2:	48 89 75 e0          	mov    %rsi,-0x20(%rbp)
 6e6:	48 8b 45 e8          	mov    -0x18(%rbp),%rax
 6ea:	8b 00                	mov    (%rax),%eax
 6ec:	89 45 fc             	mov    %eax,-0x4(%rbp)
 6ef:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
 6f3:	8b 10                	mov    (%rax),%edx
 6f5:	48 8b 45 e8          	mov    -0x18(%rbp),%rax
 6f9:	89 10                	mov    %edx,(%rax)
 6fb:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
 6ff:	8b 55 fc             	mov    -0x4(%rbp),%edx
 702:	89 10                	mov    %edx,(%rax)
 704:	90                   	nop
 705:	5d                   	pop    %rbp
 706:	c3                   	retq   
```

`00000000000006da`是函数的地址，`<swap>`是函数名，整个汇编文件分为三列，分别是指令地址、指令机器码、指令机器码反汇编得到的指令。

## backtrace

在Linux上的C/C++编程环境下，我们可以通过如下三个函数来获取程序的调用栈信息。

```c
#include <execinfo.h>
/* Store up to SIZE return address of the current program state in
   ARRAY and return the exact number of values stored.  */
int backtrace(void **array, int size);
 
/* Return names of functions from the backtrace list in ARRAY in a newly
   malloc()ed memory block.  */
char **backtrace_symbols(void *const *array, int size);
 
/* This function is similar to backtrace_symbols() but it writes the result
   immediately to a file.  */
void backtrace_symbols_fd(void *const *array, int size, int fd);
```

使用它们的时候有一下几点需要我们注意的地方：

- `backtrace`的实现依赖于栈指针（`fp`寄存器），在`gcc`编译过程中任何非零的优化等级（`-On`参数）或加入了栈指针优化参数`-fomit-frame-pointer`后多将不能正确得到程序栈信息；
- `backtrace_symbols`的实现需要符号名称的支持，在`gcc`编译过程中需要加入`-rdynamic`参数；
- 内联函数没有栈帧，它在编译过程中被展开在调用的位置；
- 尾调用优化（`Tail-call Optimization`）将复用当前函数栈，而不再生成新的函数栈，这将导致栈信息不能正确被获取。



# 崩溃定位

在程序崩溃时，系统会发送信号，在注册的信号处理函数中，将进程的`maps`文件保存下来，同时记录此时的函数调用链和`PC`指针，利用这些信息就可以进行故障定位。这里



maps文件

backtrace

PC指针





# 参考文献

1. [跟我一起写Makefile](https://seisman.github.io/how-to-write-makefile/index.html)
2. [linux进程地址空间](https://blog.csdn.net/wzd547191555/article/details/123562888)
3. [/proc/{pid}/maps简要分析](https://www.cnblogs.com/arnoldlu/p/10272466.html)
4. [Linux利用maps文件和反汇编定位崩溃地址](https://blog.csdn.net/secretii/article/details/118555867)
5. [Linux信号处理程序的一个应用定位故障](https://blog.csdn.net/qq_34999565/article/details/112342905)
6. [在Linux中如何利用backtrace信息解决问题](https://blog.csdn.net/jxgz_leo/article/details/53458366)
7. [objdump(Linux)反汇编命令使用指南](https://blog.csdn.net/qq_45477402/article/details/124450645)
8. [backtrace实现原理](https://blog.csdn.net/kz01081/article/details/103265807)
9. [addr2line 动态库](https://blog.csdn.net/qq_39852676/article/details/122523274)

