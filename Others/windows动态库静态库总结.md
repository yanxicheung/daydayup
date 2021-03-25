梳理下项目中常用的动态库、静态库使用方式。
# 静态库

静态库工程生成一个`静态库lib`文件。

参考文献6制作lib后，有三种方式去使用：

1. 使用`comment` 显式调用，这样不用在`visual studio` `链接器-`>`输入`->`附件依赖库`里面配置依赖库，由于没有指定依赖库路径所有需要显示填写路径。
2. 使用`comment` 显式调用，同时需要在`链接器-`>`常规`->`附加库目录`中配置依赖库路径。由于使用了`comment`所以不用在`visual studio` `链接器-`>`输入`->`附件依赖库`里面配置。
3. 隐式调用，需要在`链接器-`>`常规`->`附加库目录`中配置依赖库路径，同时在`链接器-`>`常规`->`附加库目录`中配置依赖库路径。

```c++
#include <iostream>
#include "Mylib.h"                                // 工程配置里面设置头文件包含路径

//#pragma comment(lib,"../Debug/staticlib.lib")   // 指定路径，当前路径为vcxproj文件所在路径
//#pragma comment(lib,"staticlib.lib")            //工程配置里面可以配置，就可以不填写路径了;(链接器附加库目录中配置)
// 可以在visual studio里面配置
using namespace std;
int main(int argc, char **argv)
{
	STATIC_LIB::PrintHello();
	system("pause");
	return 0;
}
```



# 动态库

动态库工程生成一个`引入库lib`文件和dll文件。对一个DLL文件来说，其引入库文件（.lib）包含该DLL导出的函数和变量的符号名，而.dll文件包含该DLL实际的函数和数据。动态库的引入库文件和静态库文件有着本质的区别。

代码里面设置导出符号：

```c++
//MyLib.h
#pragma once
#ifndef _STATIC_LIB_H_
#define _STATIC_LIB_H_
#include <iostream>
namespace STATIC_LIB
{
	extern "C" __declspec(dllexport) void __cdecl  PrintHello();
}
#endif

// MyLib.cpp
#include "MyLib.h"
extern "C" __declspec(dllexport) void __cdecl STATIC_LIB::PrintHello()
{
	std::cout << "Hello world!" << std::endl;
}
```

动态库使用方式有两种：一种是“隐式调用”，另外一种是“显示调用”。

## 隐式调用：

需要引入库.lib文件，同时在`visual studio`里面配置。需要在`链接器-`>`常规`->`附加库目录`中配置依赖库路径，同时在`链接器-`>`常规`->`附加库目录`中配置依赖库路径。

```c++
#include <iostream>
#include "Mylib.h"  // 需要include头文件
using namespace std;
int main(int argc, char **argv)
{
	STATIC_LIB::PrintHello();
	system("pause");
	return 0;
}
```



## 显示调用：

不需要使用引入库.lib文件，也不需要include头文件，直接使用函数加载库：

```c++
#include <iostream>
//#include "Mylib.h"  // 不用include头文件;
#include <windows.h>
using namespace std;
typedef void(*func)(void);
int main(int argc, char **argv)
{
	HMODULE hDll = LoadLibrary("staticlib.dll");
	if (hDll != NULL)
	{
		func f = (func)GetProcAddress(hDll, "PrintHello");  // 名字要一致，否则返回空指针;
		if (f != NULL)
		{
			f();
		}
	}
	FreeLibrary(hDll);
	system("pause");
	return 0;
}
```




# 参考文献：

1. [dll 导出函数名的那些事](https://www.cnblogs.com/fishseewater/archive/2010/10/05/1844292.html)
2. [windows 动态库的封装以及调用](https://www.cnblogs.com/nanqiang/p/10049092.html)
3. [如何制作windows下的dll(动态链接库)以及如何使用DLL文件](https://blog.csdn.net/qq_28107929/article/details/85631555)
4. [Dll制作与使用教程](https://blog.csdn.net/e295166319/article/details/78295470)
5. [DLL编写教程](http://www.blogjava.net/wxb_nudt/archive/2007/09/11/144371.html)
6. [手把手教你如何制作和使用lib和dll](https://blog.csdn.net/sj2050/article/details/81700183)
7. [windows中静态库lib和动态dll的区别及使用方法](https://blog.csdn.net/dxzysk/article/details/66477147)
