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

## 静态模式：

静态模式可以更加容易地定义多目标的规则，可以让我们的规则变得更加的有弹性和灵活。我们还是先来看一下语法：

```shell
<targets ...> : <target-pattern> : <prereq-patterns ...>
    <commands>
    ...
```

例子如下：

```Makefile
objects = foo.o bar.o

all: $(objects)

$(objects): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@
```

如果我们的 `%.o` 有几百个，静态模式规则可以写完一堆规则，实在是太有效率了。

## 同名目标：

```Makefile
target1: dep1

target1: dep2
	cmd2
```

这种情况下，这两个相同的target1会被合并成:

```Makefile
target1: dep1 dep2
	cmd2
```

但如果第一条规则本身也带一个命令的话， makefile就无法合并， 给出警告，并用后面的规则替代前面的规则:

```makefile
target1: dep1
	cmd1
target1: dep2
	cmd2
```

最后生成的是, 其实就是后一条替代了前一条，然后给出警告：

```makefile
target1: dep2
	cmd2
```

可以参考[Makefile 的重复目标-arley-ChinaUnix博客](http://blog.chinaunix.net/uid-21778123-id-1815467.html)

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

**功能**：这个函数的意思是，把参数 `` 中的单词逐一取出放到参数 `` 所指定的变量中，然后再执行 `` 所包含的表达式。每一次 `` 会返回一个字符串，循环过程中， `` 的所返回的每个字符串会以空格分隔，最后当整个循环结束时， `` 所返回的每个字符串所组成的整个字符串（以空格分隔）将会是`foreach`函数的返回值。

**示例**：

```makefile
names := a b c d
files := $(foreach n,$(names),$(n).o)
```

上面的例子中， `$(name)` 中的单词会被挨个取出，并存到变量 `n` 中， `$(n).o` 每次根据 `$(n)` 计算出一个值，这些值以空格分隔，最后作为`foreach`函数的返回，所以， `$(files)` 的值是 `a.o b.o c.o d.o` 。

### info

**格式**：$(info text...)

**功能**：打印处text的内容，相当于printf，常用于调试

**示例**：$(info "some text")打印 "some text"



### filter

**格式**：$(filter SUFFIX…,$(SOURCES))

**功能**：目标串中找出符合匹配规则的

**示例**：

```makefile
sources := foo.c bar.c baz.s ugh.h 
foo: $(sources) 
cc $(filter %.c %.s,$(sources)) -o foo 

#使用“$(filter %.c %.s,$(sources))”的返回值给 cc 来编译生成目标“foo”，函数返回
#值为“foo.c bar.c baz.s” 
```



### filter-out

**格式**：$(filter-out SUFFIX…,$(SOURCES))

**功能**：从目标串中过滤掉符合匹配规则的

**示例**：

```Makefile
objects=main1.o foo.o main2.o bar.o 
mains=main1.o main2.o 
$(filter-out $(mains),$(objects)) 

#实现了去除变量“objects”中“mains”定义的字串（文件名）功能。它的返回值
#为“foo.o bar.o”。
```



## 自动变量：

$@ ：表示目标文件

$^ ：表示所有的依赖文件

$< ：表示第一个依赖文件

$? ：表示比目标还要新的依赖文件列表



## 目标变量：

为某个目标设置局部变量，这种变量被称为“Target-specific Variable”，它可以和“全局变量”同名，因为它的作用范围只在这条规则以及连带规则中，所以其值也只在作用范围内有效。而不会影响规则链以外的全局变量的值。

其语法是：

```
<target ...> : <variable-assignment>;

<target ...> : overide <variable-assignment>
```

`variable-assignment`;可以是前面讲过的各种赋值表达式，如 `=` 、 `:=` 、 `+=` 或是 `?=` 。第二个语法是针对于make命令行带入的变量，或是系统环境变量。

这个特性非常的有用，当我们设置了这样一个变量，这个变量会作用到由这个目标所引发的所有的规则中去。如：

```Makefile
prog : CFLAGS = -g
prog : prog.o foo.o bar.o
    $(CC) $(CFLAGS) prog.o foo.o bar.o

prog.o : prog.c
    $(CC) $(CFLAGS) prog.c

foo.o : foo.c
    $(CC) $(CFLAGS) foo.c

bar.o : bar.c
    $(CC) $(CFLAGS) bar.c
```

在这个示例中，不管全局的 `$(CFLAGS)` 的值是什么，在prog目标，以及其所引发的所有规则中（prog.o foo.o bar.o的规则）， `$(CFLAGS)` 的值都是 `-g`。

还有一个具体的示例：[recipes/Makefile at master · chenshuo/recipes (github.com)](https://github.com/chenshuo/recipes/blob/master/thread/Makefile)



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
8. [Makefile 语法及使用笔记_丨匿名用户丨的博客-CSDN博客_makefile判断文件大小](https://blog.csdn.net/p1279030826/article/details/126011392)

