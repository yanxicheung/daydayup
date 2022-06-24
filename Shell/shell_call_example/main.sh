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