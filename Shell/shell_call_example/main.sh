#!/bin/bash

a=main

#export����������ڴ���һ�����߶��������ֵ���κκ����Ľű�
#child.sh����Ҫʹ��,����Ҫexport
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


echo "now return back to main"    # exec��ʽִ��,��俪ʼ������ִ��;
echo "PID for main after child.sh : $$"
echo "now main.sh a is ===> $a"
echo "echo ===> $b"    # ��exec��ʽִ��child.sh����b������Ч��,���Դ�ӡ����



#�ܽ�
#�����õ��ӽű�����̳е�ǰ���ű��Ļ�������;


#fork��ʽִ�нű�,���Ե�ǰshellΪ������,�����µĽ���,ִ����Ϻ�,���ű����ᱣ���ӽű�������������������(���ӽű��ı���������ظ�shell)

#exec��ʽִ�нű���������µĽ��̺�,���ǻḲ�������̵Ĵ���(�����ò�����)

#source��ʽִ�нű���������µĽ��̺�,�൱��include,��Ҫִ�еĽű������ݶ����ƹ���,�ӽű�ӵ�еĻ������������������ᱻ��ǰshell����,ִ����Ϻ�ԭ��shell��������.