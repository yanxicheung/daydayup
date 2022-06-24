# 基础：

`shell`是基于过程式、解释执行的语言。

`shell`是动态编译语言，不用事先声明，可随时改变类型。

`shell`是弱类型语言，语言的运行时会隐式做数据类型转换。无须指定类型，默认均为字符型；参与运算会自动进行隐式类型转换。



格式要求：首行`shebang`机制

```bash
#!/bin/bash
```



检测脚本语法错误：

```
bash -n /path/to/some_script
```



调试执行：

```
bash -x /path/to/some_script
```



# 变量：

变量无须事先定义可直接调用。

在`shell`定义变量中，每一个变量的值都是字符串，即使将整数和小数赋值给变量，它们也会被视为字符串，这一点和大部分的编程语言不同。

赋值号`=`的两边不能有空格。只在使用变量时加`$`，赋值时不能加。

如果赋给变量的值不包含任何空白符（例如空格、Tab 缩进等），那么可以不使用引号；如果赋给变量的值包含了空白符，那么就必须使用引号包围起来。

```bash
#!/bin/bash

#*************
#author: danielzpp
#变量的定义和使用
#*************

#在bash shell定义变量,每一个变量的值都是字符串,即使将整数和小数赋值给变量,它们也会被视为字符串,这一点和大部分的编程语言不同.
a=123+1

#使用一个定义过的变量,只要在变量名前面加美元符号$即可
echo $a     #打印出123+1

#以单引号' '包围变量的值时,单引号里面是什么就输出什么,即使内容中有变量也会把它们原样输出.也称作强引用
echo '$a'   #打印出$a

#以双引号" "包围变量的值时,输出时会先解析里面的变量和命令,而不是把双引号中的变量名原样输出.也称作弱引用
echo "$a"   #打印出123+1

#｀｀与$() 命令替换
#｀｀和$() 内部通常是一个shell命令，在执行脚本之前，会先执行这个命令，相当于函数调用
b=`ls -l`
echo "$b"    #先执行ls -l 然后打印结果

b=$(ls -al)
echo "$b"

echo `date`  #打印日期

#shell内建变量
echo $PATH
echo $SHELL
echo $LD_LIBRARY_PATH


#总结:如果变量的内容是数字,可以不加引号;
#如果需要原样输出就加单引号;
#其他没有特别要求的字符串等最好都加上双引号;
#定义变量时加双引号是最常见的使用场景.
```



## 变量引用：

一般情况下，`$var`与`${var}`并没有区别，但是用`${ }`会比较精确的界定变量名称的范围，比方说：

```shell
A="dablelv"
echo $AB
```

原本是打算先将`$A`的结果替换出来，然后再补一个B字母于其后，但在命令行上，真正的结果却是只会替换变量名称为AB的值出来。若使用`${}`就没问题了。

```bash
echo ${A}B
```



## 位置变量：

位置变量是在`shell`中内置的变量, 在脚本代码中调用通过命令行传递给脚本的参数。

```bash
#!/bin/bash
#*************
#author: danielzpp
#位置变量演示
#*************


cmd=$0   #命令本身
num=$#   #参数个数,不包括命令本身

COLOR="\033[1;31m"
END="\033[0m"

echo -e "HOSTNAME: $COLOR `hostname` $END"   # -e:激活转义字符
echo "the num of arg is $num"
echo "the num of arg is $#"

if [ $# -ne 3 ];then
	echo "input 3 args"
	exit
fi

arg1=$1
arg2=$2
arg3=$3

echo $arg1
echo $arg2
echo $arg3
echo $@
echo $*

#在没有双引号包裹时，$*与 $@相同：都是数组
for elem in $@
do 
	echo $elem
done

for elem in $*
do 
	echo $elem
done

#被双引号包裹时，$*与 $@不同："$@"为数组，"$*"为一个字符串
for elem in "$@"
do 
	echo $elem
done

for elem in "$*"
do 
	echo $elem
done
```



# 其他：

进程执行后，将使用变量 `$?` 保存状态码的相关数字，不同的值反应成功或失败，`$?`取值范例 0-255 

```bash
#$?的值为0 代表成功
#$?的值是1到255   代表失败
```



# 算术运算



# 逻辑运算



# 条件测试	

## 变量

## 数值

## 字符

## 文件 

存在性 、权限   、属性



# 调用外部脚本方式：

被调用的子脚本都会继承当前父脚本的环境变量。

| 方式   | 是否产生新进程 | 子脚本是否返回 | 子脚本环境变量是否带回父脚本 |
| ------ | -------------- | -------------- | ---------------------------- |
| fork   | 是             | 是             | 否                           |
| exec   | 否             | 否             | 否                           |
| source | 否             | 是             | 是                           |



```bash
#!/bin/bash

a=main

#export命令可以用于传递一个或者多个变量的值到任何后续的脚本
#child.sh里面要使用,所以要export
export a  
echo "a in main is===> $a"
echo "PID for main before child is $$"
case $1 in
	exec)
		echo "using exec"
		exec ./child.sh ;;
	source)
		echo "using sourceing"
		source ./child.sh ;;
	*)
		echo "using fork"
		./child.sh ;;
esac

echo "now return back to main"    # exec方式执行,这句开始都不会执行;
echo "PID for main after child.sh : $$"
echo "now main.sh a is ===> $a"
echo "echo ===> $b"    # 以exec方式执行child.sh变量b才是有效的,可以打印出来

#总结
#被调用的子脚本都会继承当前父脚本的环境变量;
#fork方式执行脚本,会以当前shell为父进程,产生新的进程,执行完毕后,主脚本不会保留子脚本环境变量和声明变量(即子脚本的变量不会带回父shell)
#exec方式执行脚本不会产生新的进程号,但是会覆盖主进程的代码(即调用不返回)
#source方式执行脚本不会产生新的进程号,相当于include,把要执行的脚本的内容都复制过来,子脚本拥有的环境变量和声明变量会被当前shell保留,执行完毕后原主shell继续运行.
```



```bash
#!/bin/bash

echo "I am child.sh"
echo "PID for child.sh is $$"

echo "child.sh get a from main is ===>$a"

a=child.sh

b=foo

echo "now child.sh a is ===>$a"
echo "now child.sh exec end"
```



# 习题：

```
1、编写脚本 systeminfo.sh，显示当前主机系统信息，包括:主机名，IPv4地址，操作系统版本，内核版本，CPU型号，内存大小，硬盘大小
2、编写脚本 backup.sh，可实现每日将/etc/目录备份到/backup/etcYYYY-mm-dd中
3、编写脚本 disk.sh，显示当前硬盘分区中空间利用率最大的值
4、编写脚本 links.sh，显示正连接本主机的每个远程主机的IPv4地址和连接数，并按连接数从大到小排序
```



```
1、编写脚本 argsnum.sh，接受一个文件路径作为参数；如果参数个数小于1，则提示用户“至少应该给一个参数”，并立即退出；如果参数个数不小于1，则显示第一个参数所指向的文件中的空白行数
2、编写脚本 hostping.sh，接受一个主机的IPv4地址做为参数，测试是否可连通。如果能ping通，则提示用户“该IP地址可访问”；如果不可ping通，则提示用户“该IP地址不可访问” 
3、编写脚本 checkdisk.sh，检查磁盘分区空间和inode使用率，如果超过80%，就发广播警告空间将满
4、编写脚本 per.sh，判断当前用户对指定参数文件，是否不可读并且不可写
5、编写脚本 excute.sh ，判断参数文件是否为sh后缀的普通文件，如果是，添加所有人可执行权限，否则提示用户非脚本文件
6、编写脚本 nologin.sh和 login.sh，实现禁止和允许普通用户登录系统
```



# 参考文献：

1. [shell变量 $*与 $@的区别](https://blog.csdn.net/ababab12345/article/details/123503009)
2. [Linux shell脚本中调用另一个shell(exec、source、fork)](https://blog.51cto.com/qujunorz/1541676)
3. [Linux Shell的脚本提示 “source: not found” 解决方法](https://blog.csdn.net/qq_44732146/article/details/124664543)