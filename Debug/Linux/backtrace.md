# 背景

最近项目中用到了一个库，在程序崩溃时可以生成`exception`文件，记录程序崩溃时的调用信息，对于定位问题比较有价值，因此整理下这个库涉及到的知识点。相关测试代码已经放到[`github`](https://github.com/yanxicheung/daydayup/tree/main/Debug/Linux)可以下载调试。



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



## signal

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

## strip

实际项目中，许多组件编译后，都会使用`strip`命令减小目标文件的大小，处理后的文件依然可以正常运行，但是其中的符号信息（比如函数名）会失去。出问题后不利于定位。

解决方法是在编译（`gcc -c`）阶段加入`-rdynamic`选项，此方法会将函数名加入到`*.dyn`节中，`strip`对其无效。

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

在程序崩溃时，系统会发送信号，在注册的信号处理函数中，将进程的`maps`文件保存下来，同时记录此时的函数调用链，利用这些信息就可以进行故障定位。前提是需要添加编译选项`-g`（不加也没事，不过用`addr2line`获得崩溃代码的行号需要），链接选项`-rdynamic`（**一定要加**） 。

## 在可执行文件中崩溃

在64位`Linux`上编译运行`example`下的`test`，程序崩溃：

```shell
hello world
segmentfault addr 0x55daa333903e
=========>>>maps <<<=========
55daa3338000-55daa333a000 r-xp 00000000 08:01 24786361  /example/test
55daa3539000-55daa353a000 r--p 00001000 08:01 24786361  /example/test
55daa353a000-55daa353b000 rw-p 00002000 08:01 24786361  /example/test
55daa3e40000-55daa3e61000 rw-p 00000000 00:00 0         [heap]
7f09107eb000-7f09109d2000 r-xp 00000000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f09109d2000-7f0910bd2000 ---p 001e7000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f0910bd2000-7f0910bd6000 r--p 001e7000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f0910bd6000-7f0910bd8000 rw-p 001eb000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f0910bd8000-7f0910bdc000 rw-p 00000000 00:00 0 
7f0910bdc000-7f0910c03000 r-xp 00000000 08:01 10490421  /lib/x86_64-linux-gnu/ld-2.27.so
7f0910de4000-7f0910de6000 rw-p 00000000 00:00 0 
7f0910e03000-7f0910e04000 r--p 00027000 08:01 10490421  /lib/x86_64-linux-gnu/ld-2.27.so
7f0910e04000-7f0910e05000 rw-p 00028000 08:01 10490421  /lib/x86_64-linux-gnu/ld-2.27.so
7f0910e05000-7f0910e06000 rw-p 00000000 00:00 0 
7ffc3e767000-7ffc3e788000 rw-p 00000000 00:00 0         [stack]
7ffc3e79e000-7ffc3e7a1000 r--p 00000000 00:00 0         [vvar]
7ffc3e7a1000-7ffc3e7a3000 r-xp 00000000 00:00 0         [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0 [vsyscall]

=========>>>catch signal 11 <<<=========   # 信号11是段错误
Dump stack start...
backtrace() returned 7 addresses
  [00] ./test(dump+0x2e) [0x55daa3338d98]
  [01] ./test(signal_handler+0xb8) [0x55daa3338f2a]
  [02] /lib/x86_64-linux-gnu/libc.so.6(+0x3ef20) [0x7f0910829f20]
  [03] ./test(segmentfault+0x3a) [0x55daa3339078]   #这里崩溃
  [04] ./test(main+0x36) [0x55daa3338f9c]
  [05] /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xe7) [0x7f091080cb97]
  [06] ./test(_start+0x2a) [0x55daa3338c8a]
Dump stack end...
Segmentation fault (core dumped)
```



由于64位系统运行的可执行文件的[符号表](https://so.csdn.net/so/search?q=符号表&spm=1001.2101.3001.7020)地址和实际运行时地址差异甚大。

崩溃地址`0x55daa3339078`是动态映射的虚拟地址，该虚拟地址是通过符号表地址+该代码段映射区间（maps里面有）的地址得来的。

`0x55daa3339078`落在区间`55daa3338000-55daa333a000`

得到真正的符号表地址`0x55daa3339078`-`0x55daa3338000`=`0x1078`

```
daniel@daniel:~/example$  addr2line -e test 1078
～/example/calc.c:44
```

32位系统显示的是实际地址，可以不用转换。



上面是获得符号表地址的一种方法，也可以使用`objdump -d test`将`test`反汇编找到`segmentfault`的地址:

```shell
000000000000103e <segmentfault>:
    103e:	55                   	push   %rbp
    103f:	48 89 e5             	mov    %rsp,%rbp
    1042:	48 83 ec 10          	sub    $0x10,%rsp
    1046:	48 8d 35 f1 ff ff ff 	lea    -0xf(%rip),%rsi   # 103e <segmentfault>
    104d:	48 8d 3d b1 01 00 00 	lea    0x1b1(%rip),%rdi  # 1205 <_IO_stdin_used+0xe5>
    1054:	b8 00 00 00 00       	mov    $0x0,%eax
    1059:	e8 a2 fb ff ff       	callq  c00 <printf@plt>
    105e:	c7 45 f0 0a 00 00 00 	movl   $0xa,-0x10(%rbp)
    1065:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%rbp)
    106c:	48 c7 45 f8 00 00 00 	movq   $0x0,-0x8(%rbp)
    1073:	00 
    1074:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
    1078:	c7 00 01 00 00 00    	movl   $0x1,(%rax)
    107e:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
    1082:	8b 10                	mov    (%rax),%edx
    1084:	8b 45 f0             	mov    -0x10(%rbp),%eax
    1087:	01 d0                	add    %edx,%eax
    1089:	89 45 f4             	mov    %eax,-0xc(%rbp)
    108c:	8b 45 f4             	mov    -0xc(%rbp),%eax
    108f:	c9                   	leaveq 
    1090:	c3                   	retq   
    1091:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
    1098:	00 00 00 
    109b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
```

真正的符号表地址为`000000000000103e` + `0x3a` = `0x1078`



## 在动态库中崩溃

```shell
hello world
add(1,2)=3
segmentfault addr 0x7f7335fb483a

=========>>>maps <<<=========
5631f8167000-5631f8169000 r-xp 00000000 08:01 24786364     /example/test_dynamic
5631f8368000-5631f8369000 r--p 00001000 08:01 24786364     /example/test_dynamic
5631f8369000-5631f836a000 rw-p 00002000 08:01 24786364     /example/test_dynamic
5631f8706000-5631f8727000 rw-p 00000000 00:00 0            [heap]
7f7335bc3000-7f7335daa000 r-xp 00000000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f7335daa000-7f7335faa000 ---p 001e7000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f7335faa000-7f7335fae000 r--p 001e7000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f7335fae000-7f7335fb0000 rw-p 001eb000 08:01 10490449  /lib/x86_64-linux-gnu/libc-2.27.so
7f7335fb0000-7f7335fb4000 rw-p 00000000 00:00 0 
7f7335fb4000-7f7335fb5000 r-xp 00000000 08:01 24786363  /example/libcalc.so
7f7335fb5000-7f73361b4000 ---p 00001000 08:01 24786363  /example/libcalc.so
7f73361b4000-7f73361b5000 r--p 00000000 08:01 24786363  /example/libcalc.so
7f73361b5000-7f73361b6000 rw-p 00001000 08:01 24786363   /example/libcalc.so
7f73361b6000-7f73361dd000 r-xp 00000000 08:01 10490421   /lib/x86_64-linux-gnu/ld-2.27.so
7f73363bb000-7f73363be000 rw-p 00000000 00:00 0 
7f73363db000-7f73363dd000 rw-p 00000000 00:00 0 
7f73363dd000-7f73363de000 r--p 00027000 08:01 10490421   /lib/x86_64-linux-gnu/ld-2.27.so
7f73363de000-7f73363df000 rw-p 00028000 08:01 10490421   /lib/x86_64-linux-gnu/ld-2.27.so
7f73363df000-7f73363e0000 rw-p 00000000 00:00 0 
7fffe6dfd000-7fffe6e1e000 rw-p 00000000 00:00 0                          [stack]
7fffe6fd4000-7fffe6fd7000 r--p 00000000 00:00 0                          [vvar]
7fffe6fd7000-7fffe6fd9000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]

=========>>>catch signal 11 <<<=========   # 信号11是段错误
Dump stack start...
backtrace() returned 7 addresses
  [00] ./test_dynamic(dump+0x2e) [0x5631f8167cc8]
  [01] ./test_dynamic(signal_handler+0xb8) [0x5631f8167e5a]
  [02] /lib/x86_64-linux-gnu/libc.so.6(+0x3ef20) [0x7f7335c01f20]
  [03] ./libcalc.so(segmentfault+0x3d) [0x7f7335fb4877]   #在动态库里面崩溃
  [04] ./test_dynamic(main+0x58) [0x5631f8167eee]
  [05] /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xe7) [0x7f7335be4b97]
  [06] ./test_dynamic(_start+0x2a) [0x5631f8167bba]
Dump stack end...
Segmentation fault (core dumped)
```

同样地获得符号表地址：`0x7f7335fb4877 - 0x7f7335fb4000 = 0x877`

```shell
daniel@daniel:~/example$ addr2line -e libcalc.so 877
/example/calc.c:44
```

`objdump -d libcalc.so`获得`segmentfault`符号地址`000000000000083a`，加上偏移`0x3d`得到 `0x877`

```shell
000000000000083a <segmentfault>:
 83a:	55                   	push   %rbp
 83b:	48 89 e5             	mov    %rsp,%rbp
 83e:	48 83 ec 10          	sub    $0x10,%rsp
 842:	48 8b 05 9f 07 20 00 	mov    0x20079f(%rip),%rax        # 200fe8 <segmentfault@@Base+0x2007ae>
 849:	48 89 c6             	mov    %rax,%rsi
 84c:	48 8d 3d 46 00 00 00 	lea    0x46(%rip),%rdi        # 899 <_fini+0x9>
 853:	b8 00 00 00 00       	mov    $0x0,%eax
 858:	e8 43 fe ff ff       	callq  6a0 <printf@plt>
 85d:	c7 45 f0 0a 00 00 00 	movl   $0xa,-0x10(%rbp)
 864:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%rbp)
 86b:	48 c7 45 f8 00 00 00 	movq   $0x0,-0x8(%rbp)
 872:	00 
 873:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 877:	c7 00 01 00 00 00    	movl   $0x1,(%rax)
 87d:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
 881:	8b 10                	mov    (%rax),%edx
 883:	8b 45 f0             	mov    -0x10(%rbp),%eax
 886:	01 d0                	add    %edx,%eax
 888:	89 45 f4             	mov    %eax,-0xc(%rbp)
 88b:	8b 45 f4             	mov    -0xc(%rbp),%eax
 88e:	c9                   	leaveq 
 88f:	c3                   	retq   

```



# 参考文献

1. [跟我一起写Makefile](https://seisman.github.io/how-to-write-makefile/index.html)
2. [linux进程地址空间](https://blog.csdn.net/wzd547191555/article/details/123562888)
3. [/proc/{pid}/maps简要分析](https://www.cnblogs.com/arnoldlu/p/10272466.html)
4. [Linux利用maps文件和反汇编定位崩溃地址](https://blog.csdn.net/secretii/article/details/118555867)
5. [Linux信号处理程序的一个应用定位故障](https://blog.csdn.net/qq_34999565/article/details/112342905)
6. [在Linux中如何利用backtrace信息解决问题](https://blog.csdn.net/jxgz_leo/article/details/53458366)
7. [objdump(Linux)反汇编命令使用指南](https://blog.csdn.net/qq_45477402/article/details/124450645)
9. [addr2line 动态库](https://blog.csdn.net/qq_39852676/article/details/122523274)
9. [linux64位系统 addr2line使用](https://blog.csdn.net/C_Creator/article/details/125172325)
10. [strip命令介绍](https://blog.csdn.net/tjcwt2011/article/details/122068913)

