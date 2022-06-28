# 基础

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



# 变量

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



## 变量引用

一般情况下，`$var`与`${var}`并没有区别，但是用`${ }`会比较精确的界定变量名称的范围，比方说：

```shell
A="dablelv"
echo $AB
```

原本是打算先将`$A`的结果替换出来，然后再补一个B字母于其后，但在命令行上，真正的结果却是只会替换变量名称为AB的值出来。若使用`${}`就没问题了。

```bash
echo ${A}B
```



## 位置变量

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



# 其他

进程执行后，将使用变量 `$?` 保存状态码的相关数字，不同的值反应成功或失败，`$?`取值范例 0-255 

```bash
#$?的值为0 代表成功
#$?的值是1到255   代表失败
```



# 算术运算

shell中，无法直接进行算术运算，默认把”算术符号”当做”字符串”与两个变量的值连接在了一起，形成了一个新的字符串。

有如下方式进行算术运算：

(1) 使用let命令进行算术运算，只支持整数运算。

```bash
let a=1+2   #   = + 两边不能有空格
let a=1-2
let a=5*2
let a=5/2
b=7
c=8
let d=$b+$c
let d=${b}+${c}
```

(2) 使用expr命令进行算术运算(功能不止于算术运算)，只支持整数运算。

```bash
expr 1 + 2  #数值与运算符号之间需要用空格隔开，否则无法进行算术运算。
expr 5 - 2
expr 5 / 2
expr 5 \* 2  #使用expr命令进行乘法运算时，需要将”乘号”转义，否则会报错。
a=2
b=3
aandb=`expr $a + $b`
echo $aandb
ajiab = $(expr $a + $b)
echo $ajiab
```

(3) 使用bc命令进行算术运算，支持小数运算。

```bash
echo "1.3+2.4" | bc
echo "1.3+2.333" | bc
echo "scale=3; 8/3" | bc # 在使用”除法”时，需要指定小数点精度，否则运算结果中不会包含小数，使用scale指定小数点精度。
```

(4) 使用运算语法：$[算术表达式]，只支持整数运算。

```bash
a=$[1+2]
echo $a
b=1
c=2
echo $[$b+$c]
```

(5) 使用运算语法：$((算术表达式))，只支持整数运算。

```bash
a=$((1+2))
echo $a
b=1
c=2
echo $(($b+$c))
```

(6) 在初始化变量时，将变量定义为”整数”类型，则可直接进行整数运算。

```bash
declare -i s
s=(1+9)/2
echo $s
```



# 逻辑运算

[]相当于test命令，-a 和 -o 需要使用测试命令进行，[[ ]] 不支持。

”&&”有短路功能，也可以称为”短路与”, 而 -a没有

```bash
if [ 10 -gt 9 -a 10 -gt 8 ];then echo "true";fi
if [[ 10 -gt 9 && 10 -gt 8 ]];then echo "true";fi
if [ 10 -gt 9 ] && [ 10 -gt 6 ];then echo "true";fi    #if [ 10 -gt 9 ] -a [ 10 -gt 6 ]会语法报错
```

”||”有短路功能，也可以称为”短路或”，而-o没有

```bash
if [ 10 -gt 9 -o 10 -gt 8 ];then echo "true";fi
if [[ 10 -gt 9 || 10 -gt 8 ]];then echo "true";fi
if [ 10 -gt 9 ] || [ 10 -gt 6 ];then echo "true";fi
```



在使用”[[  ]]”时，不能使用”-a”或者”-o”对多个条件进行连接。

在使用”[  ]”时，如果使用”-a”或者”-o”对多个条件进行连接，”-a”或者”-o”必须被包含在”[ ]”之内。

在使用”[  ]”时，如果使用”&&”或者”||”对多个条件进行连接，”&&”或者”||”必须在”[ ]”之外。



# 条件测试

## 数值

```
-gt 是否大于
-ge 是否大于等于
-eq 是否等于
-ne 是否不等于
-lt 是否小于
-le 是否小于等于
```

## 字符

```
-z "STRING" 字符串是否为空，空为真，不空为假
-n "STRING" 字符串是否不空，不空为真，空为假 
= 是否等于
!= 是否不等于
> ascii码是否大于ascii码
< 是否小于
== 左侧字符串是否和右侧的PATTERN相同
 注意:此表达式用于[[ ]]中，PATTERN为通配符
=~ 左侧字符串是否能够被右侧的PATTERN所匹配
 注意: 此表达式用于[[ ]]中；扩展的正则表达式
```

## 文件 

存在性 

```
-a FILE：同 -e
-e FILE: 文件存在性测试，存在为真，否则为假
-b FILE：是否存在且为块设备文件
-c FILE：是否存在且为字符设备文件
-d FILE：是否存在且为目录文件
-f FILE：是否存在且为普通文件
-h FILE 或 -L FILE：存在且为符号链接文件
-p FILE：是否存在且为命名管道文件
-S FILE：是否存在且为套接字文件
```

权限   

```
-r FILE：是否存在且可读
-w FILE: 是否存在且可写
-x FILE: 是否存在且可执行
-u FILE：是否存在且拥有suid权限
-g FILE：是否存在且拥有sgid权限
-k FILE：是否存在且拥有sticky权限
```

属性

```
-s FILE: 是否存在且非空
-t fd: fd 文件描述符是否在某终端已经打开
-N FILE：文件自从上一次被读取之后是否被修改过
-O FILE：当前有效用户是否为文件属主
-G FILE：当前有效用户是否为文件属组
FILE1 -ef FILE2: FILE1是否是FILE2的硬链接
FILE1 -nt FILE2: FILE1是否新于FILE2（mtime）
FILE1 -ot FILE2: FILE1是否旧于FILE2
```

# 流程控制

## 条件

```bash
#!/bin/bash
#根据命令的退出状态来执行命令
echo "hello world"
if cd /usr/bin;then
	echo "cd to /usr/bin succ"
fi
```



```bash
#!/bin/bash
weight=50  # kg
high=1.68  # m

bmi=$(echo $weight/$high^2|bc)
echo $bmi

if [ $bmi -le 18 ];then
	echo "太瘦了"
elif [ $bmi -lt 24 ];then
	echo "很棒"
else
	echo "太胖了"
fi
```



```bash
#!/bin/bash
read -p "Do you agree(yes/no)?" INPUT
INPUT=`echo $INPUT | tr 'A-Z' 'a-z'`
case $INPUT in
y|yes)
	echo "You input is YES" ;;
n|no)
	echo "You input is No" ;;
*)
	echo "Input false, please input yes or no!"
esac
```



```
1、编写脚本 createuser.sh，实现如下功能：使用一个用户名做为参数，如果指定参数的用户存在，就显示其存在，否则添加之；显示添加的用户的id号等信息
2、编写脚本 yesorno.sh，提示用户输入yes或no,并判断用户输入的是yes还是no,或是其它信息
3、编写脚本 filetype.sh，判断用户输入文件路径，显示其文件类型（普通，目录，链接，其它文件类型）
4、编写脚本 checkint.sh，判断用户输入的参数是否为正整数
```



## 循环

[shell for循环](https://blog.csdn.net/u014217137/article/details/122643844)

```bash
#!/bin/sh
echo "for example"
# 打印9*9乘法表
for i in {1..9};do
	for j in `seq $i`;do
		echo -e  "${i}x${j}=$[$i*$j]\t\c"
	done
	echo 
done

echo "==========================================="

for ((i=1;i<10;++i));do
	for((j=1;j<=i;++j));do
		echo -e "${i}*${j}=$[$i*$j]\t\c"
	done
	echo
done

echo "==========================================="

for l in $(ls);do
	echo -e "${l}\t"
done


sum=0
for i in {1..100};do
	((sum+=i))
done
echo "sum=$sum"


sum=0
for n in 1 2 3 4 5 6 7;do
	echo $n
	((sum+=n))
done
echo "sum=$sum"
```



```
1、判断/var/目录下所有文件的类型
2、添加10个用户user1-user10，密码为8位随机字符
3、/etc/rc.d/rc3.d目录下分别有多个以K开头和以S开头的文件；分别读取每个文件，以K开头的输出为文件加stop，以S开头的输出为文件名加start，如K34filename stop S66filename start
4、编写脚本，提示输入正整数n的值，计算1+2+…+n的总和
5、计算100以内所有能被3整除的整数之和
6、编写脚本，提示请输入网络地址，如192.168.0.0，判断输入的网段中主机在线状态
7、打印九九乘法表
8、在/testdir目录下创建10个html文件,文件名格式为数字N（从1到10）加随机8个字母，如：1AbCdeFgH.html
9、打印等腰三角形
10、猴子第一天摘下若干个桃子，当即吃了一半，还不瘾，又多吃了一个。第二天早上又将剩下的桃子吃掉一半，又多吃了一个。以后每天早上都吃了前一天剩下的一半零一个。到第10天早上想再吃时，只剩下一个桃子了。求第一天共摘了多少
```



[shell while循环](https://blog.csdn.net/weixin_45858439/article/details/124509503)

```bash
#!/bin/bash
echo "while example"
i=0
while [ $i -le 10 ];do
	echo $i
	let i++
done


a=10
while [ $a -ge 1 ];do
	echo "${a}*${a}=$[$a*$a]"
	let a--
done
```



```
1、编写脚本，求100以内所有正奇数之和
2、编写脚本，提示请输入网络地址，如192.168.0.0，判断输入的网段中主机在线状态，并统计在线和离线主机各多少
3、编写脚本，打印九九乘法表
4、编写脚本，利用变量RANDOM生成10个随机数字，输出这个10数字，并显示其中的最大值和最小值
5、编写脚本，实现打印国际象棋棋盘
6、后续六个字符串：efbaf275cd、4be9c40b8b、44b2395c46、f8c8873ce0、b902c16c8b、ad865d2f63是通过对随机数变量RANDOM随机执行命令： echo $RANDOM|md5sum|cut –c1-10 后的结果，请破解这些字符串对应的RANDOM值
```



```
1、每隔3秒钟到系统上获取已经登录的用户的信息；如果发现用户hacker登录，则将登录时间和主机记录于日志/var/log/login.log中,并退出脚本
2、随机生成10以内的数字，实现猜字游戏，提示比较大或小，相等则退出
3、用文件名做为参数，统计所有参数文件的总行数
4、用二个以上的数字为参数，显示其中的最大值和最小值
```



# 函数

和其他编程语言不同的是，shell函数在定义时不能指明参数，但在调用时却可以传递参数，并且给它传递什么参数它就接收什么参数。

## 定义

```bash
#语法一：
func_name （）{
 ...函数体...
}

#语法二：
function func_name {
 ...函数体...
} 

#语法三：
function func_name （） {
 ...函数体...
}
```



```bash
#和其他编程语言不同的是，shell函数在定义时不能指明参数，但在调用时却可以传递参数，并且给它传递什么参数它就接收什么参数
#!/bin/bash

if [ $# -ne 1 ];then
	echo "please input a positive num"
	exit
fi

function factorial(){
	factorial=1
	for((i=1;i<=$1;++i))
		do
			factorial=$[ $factorial * $i ]
		done
	echo $factorial
}

ret=`factorial $1`   #函数调用
echo $ret


function helloworld(){
	echo "hello world"
}

helloworld        #函数调用
ret=`helloworld`  #函数调用
echo $ret
```



## 返回值

使用return返回：取值范围固定为0~255，如果大于255会溢出。return的值**一般作为函数执行的状态**，0成功，1失败，可以通过`$?`来获取。

使用echo返回：返回函数的计算结果。

```bash
function get_sum(){
	sum=0
	for num in $@;do
		((sum+=num))
	done
	echo $sum
}

sum=`get_sum 1 2 300`   #函数调用
echo "sum1=$?"          # $?可以获取函数的退出状态(返回值)
echo $sum

sum=$(get_sum 1 2 3 4 5 6 7)  #函数调用
echo "sum2=$?"
echo $sum
```



# 数组

```bash
#!/bin/bash
echo "show array"
index=(1 2 3 4 5 6)   #定义

for((i=0;i<${#index[@]};++i));do  #遍历
	echo ${index[i]}
done

for i in ${index[@]};do  #遍历
	echo $i
done


echo ${index[0]}   # 引用
echo "len = ${#index[@]}"


function printarray(){
	arr=$1
	for i in ${arr[@]};do
		echo $i
	done
}

echo "================="
printarray "${index[*]}"   #向函数传递数组
```



# 字符串



# bash中 [ ] 与 [[ ]] 的区别:

## 判断变量是否为空

当使用”-n”或者”-z”这种方式**判断变量**是否为空时，”[ ]”与”[[  ]]”是有区别的。

使用”[ ]”时需要在变量的外侧加上双引号，与test命令的用法完全相同，使用”[[  ]]”时则不用。



## 组合判断条件

在使用”[[  ]]”时，不能使用”-a”或者”-o”对多个条件进行连接。

在使用”[  ]”时，如果使用”-a”或者”-o”对多个条件进行连接，”-a”或者”-o”必须被包含在”[ ]”之内。

在使用”[  ]”时，如果使用”&&”或者”||”对多个条件进行连接，”&&”或者”||”必须在”[ ]”之外。



# 脚本调用方式

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



同样地在终端里面调用一个shell脚本时，通常使用的方式是，即fork方式：

```bash
./run.sh
```

这样执行脚本的话终端会产生一个子`shell`，子`shell`会继承终端的环境变量，一旦子`shell`执行完毕，子`shell`中的变量和操作全部都回收，回到终端就看不到这个过程的变化。比如按照 `./run.sh`的方式执行以下脚本，执行完毕后目录没有切换成功:

```bash
#!/bin/bash
echo "hello world"
if cd /usr/bin;then
	echo "cd to /usr/bin succ"
fi
```

使用`. ./run.sh`或者 `source ./run.sh`方式执行脚本，执行完毕后目录成功切换。



# 习题

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



# 参考文献

1. [shell变量 $*与 $@的区别](https://blog.csdn.net/ababab12345/article/details/123503009)
2. [Linux shell脚本中调用另一个shell(exec、source、fork)](https://blog.51cto.com/qujunorz/1541676)
3. [Linux Shell的脚本提示 “source: not found” 解决方法](https://blog.csdn.net/qq_44732146/article/details/124664543)
4. [Shell中进行算术运算的各种方法](https://www.zsythink.net/archives/1145)
5. [Shell中组合命令的使用方法](https://www.zsythink.net/archives/1166)
6. [**Shell中[ ]与[[ ]]的区别**](https://www.zsythink.net/archives/2252)

