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