# 动态库名称

Linux上的shared library有三个名字，分别是：

## real name

对应文件名称，由主版本号+小版本号+build号组成。比如 `libmath.so.1.1.1234`，`lib`是`Linux`库的约定前缀，`math`是共享库名字，`so`是共享库的后缀名，`1.1.1234`的是共享库的版本号。

主版本号，代表当前动态库的版本，如果共享库的接口发生变化，那么这个版本号就要加1；

后面的两个版本号（小版本号和 build号）是用来指示库的更新迭代号，表示在接口没有改变的情况下，由于需求发生变化等因素，开发的新代码。



## so name

运行可执行文件，在加载共享库的时候，应该使用的文件名。其格式为`lib + math + .so + (主版本号)`。其只包含主版本号。可以通过`readelf -d`命令查看：

```bash
msp@msp-OptiPlex-5040:/home/zpp/test$ readelf -d libtest.so.0.0.0 | grep soname
 0x000000000000000e (SONAME)             Library soname: [libtest.so.0]
```

`so name`需要在编译时指定：

```bash
gcc -fPIC -o test.o -c test.c
gcc -shared -Wl,-soname,libtest.so.0 -o libtest.so.0.0.0 test.o
```

其中`libtest.so.0`是`so name`， `libtest.so.0.0.0`是`real name`。



## link name

是专门为应用程序在编译时的链接阶段而用的名字。这个名字就是`lib + math +.so` ,比如`libmath.so`。是不带任何版本信息的。



## 例子

```bash
#libtest.so是linkname不带版本号   在可执行文件编译链接的时候使用
lrwxrwxrwx 1 root root   12 10月 25 14:16 libtest.so -> libtest.so.0 

#libtest.so.0是soname带主版本号   可执行文件运行加载时使用
lrwxrwxrwx 1 root root   16 10月 25 14:09 libtest.so.0 -> libtest.so.0.0.0

 #libtest.so.0.0.1是realname  实际加载的动态库文件
-rwxr-xr-x 1 root root 8120 10月 25 14:08 libtest.so.0.0.0 

lrwxrwxrwx 1 root root   16 10月 25 14:13 libtest.so.1 -> libtest.so.1.0.0
-rwxr-xr-x 1 root root 8208 10月 25 14:13 libtest.so.1.0.0
-rwxr-xr-x 1 root root 8600 10月 25 14:16 main
-rw-r--r-- 1 root root   86 10月 25 13:54 main.c
-rw-r--r-- 1 root root 1384 10月 25 13:54 main.o
-rw-r--r-- 1 root root  225 10月 25 14:13 test.c
-rw-r--r-- 1 root root   87 10月 25 14:12 test.h
-rw-r--r-- 1 root root 1896 10月 25 14:13 test.o
```



其中`libtest.so`和`libtest.so.0`都是软链接，通过如下方式创建：

```bash
ln -s libtest.so.0.0.0  libtest.so.0
ln -s libtest.so.0  libtest.so
```



在可以行文件编译链接动态库时，先通过`linkname`找到最终找到`real name`，并且把其中的`soname`提取出来，写在应用程序自己的文件头的共享库字段里面。

当应用程序运行时，就会通过`soname`，结合动态链接程序`（ld.so）`，在给定的路径下加载`real name`的共享库。

# 参考文献：

1. [linux共享库版本控制及使用_绛洞花主敏明的博客-CSDN博客](https://blog.csdn.net/qq_39852676/article/details/120241838)