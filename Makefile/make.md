# 编译概述

## 编译基础：

使用`GCC`编译程序时可以分为4个阶段：

| （1）预处理（pre-processing） | -E   | .c---->.i        | -I （Include） | 将源文件生成中间文件 |
| ----------------------------- | ---- | ---------------- | -------------- | -------------------- |
| （2）编译（compiling）        | -S   | .i---->.s        |                | 将中间文件生成汇编   |
| （3）汇编（Assembling）       | -c   | .s--->.o         |                | 将汇编转换成机器代码 |
| （4）链接（Linking）          |      | .o--->可执行文件 | -L（Link）     | 汇集成可执行文件     |

## 基本用法： 

gcc  【options】 【filenames】

## 常用选项：

| -c            | 只是编译，不生成可执行文件，将.c文件生成.o文件               |
| ------------- | ------------------------------------------------------------ |
| -o outputfile | 确定输出文件的名字为outputfile                               |
| -g            | 产生gdb所需要的符号信息，用于对源代码的调试                  |
| -O            | 优化编译链接，编译链接时间会比较慢                           |
| -O2           | 比-O更好的优化编译链接，编译链接时间会更加慢                 |
| -Wall         | 输出所有警告信息                                             |
| -w            | 关闭所有警告信息                                             |
| -Idirname     | 将dirname的内容加入到程序头文件目录列表中，在预处理阶段使用。I意指Include |
| -Ldirmane     | 将dirname的目录加入到程序的库文件搜索目录列表中，这是链接中使用的参数。L意指Link |

# makefile：

makefile文件和make工具一起使用，用于控制工程项目的编译和链接，也可以用来编写手册页和程序的安装。

make工具用于解释执行makefile文件中的内容。

makefile文件中通常包含源文件和目标文件的依赖关系以及从源文件生成目标文件的规则。

make工具可以根据makefile判断哪些文件需要被重新编译，目标文件的构建顺序等。

## 规则：

在讲述makefile之前，先来粗略地看一看makefile的规则。

```makefile
target ... : prerequisites ...
    command
    ...
    ...
```

target：可以是一个object file（目标文件），也可以是一个执行文件，还可以是一个标签（label）。对于标签这种特性，在后续的“伪目标”章节中会有叙述。

prerequisites：生成该target所依赖的文件和/或target

command：该target要执行的命令（任意的shell命令），***一定要以tab开头***

这是一个文件的依赖关系，也就是说，target这一个或多个的目标文件依赖于prerequisites中的文件，其生成规则定义在command中。即prerequisites中如果有一个以上的文件比target文件要新的话，command所定义的命令就会被执行。

这就是makefile的规则，也就是makefile中最核心的内容。

## 常用函数：

函数调用，很像变量的使用，也是以 `$` 来标识的，其语法如下：

```makefile
$(<function> <arguments>)
#或者
${<function> <arguments>}
```

这里， `` 就是函数名，make支持的函数不多。 `` 为函数的参数，参数间以逗号 `,` 分隔，而函数名和参数之间以“空格”分隔。函数调用以 `$` 开头，以圆括号或花括号把函数名和参数括起。感觉很像一个变量，是不是？函数中的参数可以使用变量，为了风格的统一，函数和变量的括号最好一样，如使用 `$(subst a,b,$(x))` 这样的形式，而不是 `$(subst a,b, ${x})` 的形式。因为统一会更清楚，也会减少一些不必要的麻烦。

### patsubst

**格式**：$(patsubst pattern,replacement,text)

**名称**：模式字符串替换函数

**功能**：查找text中的单词（单词以“空格”、“Tab”或“回车”“换行”分隔）是否符合模式pattern，如果匹配的话，则以replacement替换。这里，pattern可以包括通配符“%”，表示任意长度的字串。如果replacement中也包含“%”，那么，replacement中的这个“%”将是pattern中的那个“%”所代表的字串。（可以用“\”来转义，以“%”来表示真实含义的“%”字符）

**返回**：函数返回被替换过后的字符串。

**示例**：$(patsubst %.c,%.o, a.c b.c)把字串“a.c b.c”符合模式[%.c]的单词替换成[%.o]，返回结果是“a.o b.o”

### notdir

**格式**：$(notdir  names)

**名称**：将参数中的路径去掉

**功能**：从文件名序列<names>中取出非目录部分。非目录部分是指最后一个反斜杠（“/”）之后的部分。

**返回**：返回文件名序列<names>的非目录部分。

**示例**：$(notdir src/foo.c hacks)返回值是“foo.c hacks”。



### wildcard

**格式**：$(wildcard PATTERN...) 

**功能**：扩展通配符

**示例**：$(wildcard *.c  ./foo/*.c) 返回值是“foo.c hacks”。搜索当前目录及./foo/下所有以.c结尾的文件，生成一个以空格间隔的文件名列表。




### foreach

**格式**：$(foreach <var>,<list>,<text>)

**名称**：用来做循环用

**功能**：这个函数的意思是，把参数 `` 中的单词逐一取出放到参数 `` 所指定的变量中，然后再执行 `` 所包含的表达式。每一次 `` 会返回一个字符串，循环过程中， `` 的所返回的每个字符串会以空格分隔，最后当整个循环结束时， `` 所返回的每个字符串所组成的整个字符串（以空格分隔）将会是foreach函数的返回值。

**示例**：

```makefile
names := a b c d
files := $(foreach n,$(names),$(n).o)
```

上面的例子中， `$(name)` 中的单词会被挨个取出，并存到变量 `n` 中， `$(n).o` 每次根据 `$(n)` 计算出一个值，这些值以空格分隔，最后作为foreach函数的返回，所以， `$(files)` 的值是 `a.o b.o c.o d.o` 。

### info

**格式**：$(info text...)

**功能**：打印处text的内容，相当于printf，常用于调试

**示例**：$(info "some text")打印 "some text"



## 自动变量：

$@ ：表示目标文件

$^ ：表示所有的依赖文件

$< ：表示第一个依赖文件

$? ：表示比目标还要新的依赖文件列表



# 学以致用：

## 原始版本：

```makefile
main.out:main.o tool1.o tool2.o
	gcc main.o tool1.o tool2.o -o main.out
main.o:main.c
	gcc main.c -c -o main.o
tool1.o:tool1.c
	gcc tool1.c -c -o tool1.o
tool2.o:tool2.c
	gcc tool2.c -c -o tool2.o
PHONY:clean
clean:
	rm -rf *.o *.out
```

## 加入变量：

```makefile
OBJS = main.o tool1.o tool2.o
CC = gcc
CFLAGS += -c  -Wall -g
main.out:$(OBJS)
	$(CC) $(OBJS) -o main.out
main.o:main.c
	$(CC) main.c  $(CFLAGS) -o main.o
tool1.o:tool1.c
	$(CC) tool1.c $(CFLAGS) -o tool1.o
tool2.o:tool2.c
	$(CC) tool2.c $(CFLAGS) -o tool2.o
clean:
	$(RM) *.o *.out -r
```

## 自动变量：

```makefile
OBJS = main.o tool1.o tool2.o
CC = gcc
CFLAGS += -c  -Wall -g

main.out:$(OBJS)
	$(CC) $(OBJS) -o $@
main.o:main.c
	$(CC) $^  $(CFLAGS) -o $@
tool1.o:tool1.c
	$(CC) $^  $(CFLAGS) -o $@
tool2.o:tool2.c
	$(CC) $^  $(CFLAGS) -o $@
clean:
	$(RM) *.o *.out -r
```

## 隐含规则：

```makefile
OBJS = main.o tool1.o tool2.o
CC = gcc
CFLAGS += -c  -Wall -g
main.out:$(OBJS)
	$(CC) $(OBJS) -o $@
%.o:%.c
	$(CC) $^  $(CFLAGS) -o $@
clean:
	$(RM) *.o *.out -r
```



```shell
all:clean main.out
	@echo "clean first then compile then link"
	@echo $(OBJS)

OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
CC = gcc
CFLAGS += -c  -Wall -g

main.out:$(OBJS);@echo "link"
	$(CC) $^ -o $@
	
%.o:%.c;@echo "complie"
	$(CC) $^ $(CFLAGS) -o $@

PHONY:clean
clean:
	$(RM) *.o *.out -r
```

# 练习题：

given：

```makefile
all:ef cd
	@echo 123
cd:
	@echo 456
ef:
	@echo 789
```

输出：

```shell
789
456
123
```


given：

```makefile
all:
	@echo 123
cd:
	@echo 456
ef:
	@echo 789
```

输出 :

```shell
123
```


given：

```makefile
all:clean main.out
	@echo "clean first then compile then link"
main.out:tool1.o tool2.o main.o
	@echo "link"
	gcc main.o tool1.o tool2.o -o main.out
main.o:main.c
	@echo "link"
	gcc main.c -c -o main.o
tool1.o:tool1.c
	@echo "link"
	gcc tool1.c -c -o tool1.o
tool2.o:tool2.c
	@echo "link"
	gcc tool2.c -c -o tool2.o

PHONY:clean
clean:
	rm -rf *.o *.out
```

输出：

```shell
rm -rf *.o *.out
link
gcc tool1.c -c -o tool1.o
link
gcc tool2.c -c -o tool2.o
link
gcc main.c -c -o main.o
link
gcc main.o tool1.o tool2.o -o main.out
clean first then compile then link
```

# 最佳实践：

工程目录如下，请使用makefile构建工程：

```shell
├── include
│   ├── base
│   │   ├── BaseTypes.h
│   │   ├── InterfaceDef.h
│   │   ├── Keywords.h
│   │   └── stdc.h
│   ├── BaseMacro.h
│   ├── Cent.h
│   ├── common.h
│   └── Dollar.h
├── Makefile
├── README.md
├── src
│   ├── Cent.cpp
│   ├── common.cpp
│   └── Dollar.cpp
└── test
    ├── CentTest.cpp
    ├── DollarTest.cpp
    ├── FormatTest.cpp
    └── main.cpp
```

https://github.com/yanxicheung/usd



# Q&A：

## 一个Makefile如何生成若干个可执行文件？

如果你的Makefile需要一口气生成若干个可执行文件，但你只 想简单地敲一个make完事，并且，所有的目标文件都写在一个Makefile中，那么你可以使用“伪目标”这个特性：

```makefile
all : prog1 prog2 prog3
.PHONY : all

prog1 : prog1.o utils.o
    cc -o prog1 prog1.o utils.o

prog2 : prog2.o
    cc -o prog2 prog2.o

prog3 : prog3.o sort.o utils.o
    cc -o prog3 prog3.o sort.o utils.o
```

**Makefile中的第一个目标会被作为其默认目标。**我们声明了一个“all”的伪目标，其依赖于其它三个目标。

由于默认目标的特性是，总是被执行的，但由于“all”又是一个伪目标，伪目标只是一个标签不 会生成文件，所以不会有“all”文件产生。

于是，其它三个目标的规则总是会被决议。也就达到了我们一口 气生成多个目标的目的。 `.PHONY : all` 声明了“all”这个目标为“伪目标”。

（注：这里的显式 “.PHONY : all” 不写的话一般情况也可以正确的执行，这样make可通过隐式规则推导出， “all” 是一 个伪目标，执行make不会生成“all”文件，而执行后面的多个目标。建议：显式写出是一个好习惯。）



# Refrence：

1. [linux make makefile 内置变量 默认变量](https://blog.csdn.net/whatday/article/details/104079644)
3. [makefile $@, $^, $<, $?](https://www.cnblogs.com/gamesun/p/3323155.html)
4. [跟我一起写Makefile](https://seisman.github.io/how-to-write-makefile/index.html)
4. [Makefile中.PHONY的作用](https://www.cnblogs.com/idorax/p/9306528.html)
5. [Makefile 中:= ?= += =的区别](https://www.cnblogs.com/zgq0/p/8716150.html)
6. [Makefile的静态模式%.o : %.c](https://blog.csdn.net/u012351051/article/details/88600562)
7. [makefile内置变量及自动变量](https://blog.csdn.net/hejinjing_tom_com/article/details/40781787)

