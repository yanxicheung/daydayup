自定义main函数和默认main函数,默认main函数定义为弱定义

```cpp
// 框架定义的默认的main函数
__attribute__((weak))
//! 定义为弱定义，允许用户自己定义。
//! 另一方面，避免在 make test 时与 gtest 的 main() 冲突。
int main(int argc, char **argv)
{
    printf("=================\n");
    return tbox::main::Main(argc, argv);
}

bool __attribute((weak)) LogOutput_FilterFunc(const LogContent *content);
```



日志

```
log.h是接口文件
LogPrintfFunc放在log_imp.cpp中实现

https://blog.csdn.net/u010835747/article/details/121902306
```



前后台

```

```



telnet命令

```

```



命令行解析工具

修改默认json配置

```

```



Module模块（app）

```

```



Context

```

```



EventLoop

```
signal event
timer event
fd event

std::push_heap
std::make_heap
std::pop_heap
```



定时器

```
https://blog.csdn.net/hsy12342611/article/details/127964500
```



IO事件

```

```



信号事件（安全使用信号）

```

```



telnet

```

```

