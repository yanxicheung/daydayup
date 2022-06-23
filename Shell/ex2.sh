#!/bin/bash
#*************
#author: danielzpp
#λ�ñ�����ʾ
#*************


cmd=$0   #�����
num=$#   #��������,�����������

COLOR="\033[1;31m"
END="\033[0m"

echo -e "HOSTNAME: $COLOR `hostname` $END"   # -e:����ת���ַ�
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

#��û��˫���Ű���ʱ��$*�� $@��ͬ����������
for elem in $@
do 
	echo $elem
done

for elem in $*
do 
	echo $elem
done

#��˫���Ű���ʱ��$*�� $@��ͬ��"$@"Ϊ���飬"$*"Ϊһ���ַ���
for elem in "$@"
do 
	echo $elem
done

for elem in "$*"
do 
	echo $elem
done