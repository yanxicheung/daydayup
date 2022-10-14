# 概述：

​		CMake是一个比make更高级的编译配置工具，它可以根据不同平台、不同的编译器，生成相应的Makefile或者vcproj项目。通过编写CMakeLists.txt，可以控制生成的Makefile，从而控制编译过程。

​		我们经常使用CMake自动生成的Makefile来构建项目生成目标文件，安装文件。本文主要介绍几种常见的工程结构及对应的CMakeList文件的写法。



# case1：

入门例子，所有文件都在同一个文件夹下。

源代码来自于：https://github.com/yanxicheung/CMakeNote/tree/main/CMakeDemo/case1

## 目录结构：

```cmake
.
├── CMakeLists.txt
├── main.cpp
├── MathFunctions.cpp
└── MathFunctions.h
```

## CMakeLists:

```cmake
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo2)

# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标
add_executable(Demo ${DIR_SRCS})
```



# case2：

多个目录，多个文件，使用一个CMakeLists的例子。

源代码来自于：https://github.com/yanxicheung/CMakeNote/tree/main/CMakeDemo/case2/quantity

## 目录结构：

```bash
.
├── CMakeLists.txt
├── include
│   ├── Amount.h
│   └── Length.h
├── source
│   └── Length.cpp
└── test
    ├── LengthTest.cpp
    └── main.cpp
```

## CMakeLists:

```cmake
# 设置工程名称
project(quantity) 
cmake_minimum_required(VERSION 2.8)

# 用于设置环境变量
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# 设置头文件包含路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 遍历文件
file(GLOB_RECURSE all_files
source/*.cpp
source/*.cc
source/*.c
test/*.cpp
test/*.cc
test/*.c)

# 生成目标可执行文件
add_executable(quantity-test ${all_files})

# 由于使用了gtest框架,所以需要链接这个库
target_link_libraries(quantity-test gtest)
```



# case3：

case3是一个多级目录使用CMakeLists.txt进行编译的例子。

源代码来自于：https://github.com/yanxicheung/CMakeLists

## 目录结构：

```bash
.
├── CMakeLists.txt               // 顶层CMakeList
├── hello
│   ├── CMakeLists.txt           // 会生成静态库，供顶层调用
│   ├── include
│   │   └── hello.h
│   └── source
│       └── hello.cpp
├── main.cpp
└── world
    ├── CMakeLists.txt           // 会生成静态库，供顶层调用
    ├── include
    │   └── world.h
    └── source
        └── world.cpp
```



## CMakeLists:

顶层：

```cmake
cmake_minimum_required(VERSION 2.8)

set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(hello_dir ${curr_dir}/hello)
set(world_dir ${curr_dir}/world)

project(helloworld)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Add header file include directories
include_directories(
    ${hello_dir}/include
    ${world_dir}/include
)

# Add block directories
add_subdirectory(hello)
add_subdirectory(world)
# Target
add_executable(helloworld main.cpp)
target_link_libraries(helloworld hello world)
```

子目录hello：

```cmake
set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    include
)

aux_source_directory(${curr_dir}/source DIR_HELLO_SRCS)

add_library(hello ${DIR_HELLO_SRCS})    // 默认是静态库STATIC
```

子目录world：

```cmake
set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    include
)

aux_source_directory(${curr_dir}/source DIR_WORLD_SRCS)

add_library(world ${DIR_WORLD_SRCS})   // 默认是静态库STATIC
```



# case4：

在这个示例中我们需要：

1. 为项目增加编译选项，从而可以根据用户的环境和需求选择最合适的编译方案：当ENABLE_TEST打开时生成静态库文件libcub.a并且编译单元测试，生成可执行文件。否则只生成静态库文件libcub.a
2. 将静态库文件libcub.a及头文件安装到系统目录中。

源代码来自于：https://github.com/yanxicheung/cub

## 目录结构：

```bash
.
├── build.sh
├── CMakeLists.txt            // 顶层CMakeList
├── doc
├── include
│   └── cub
│       ├── algo
│       ├── base
│       ├── cub.h
│       ├── dci
│       ├── ...
├── README.md
├── src
│   ├── CMakeLists.txt        // 会生成静态库，供顶层调用
│   ├── log
│   ├── mem
│   └── sched
└── test
    ├── CMakeLists.txt       // 会生成单元测试的可执行文件
    ├── main.cpp
    ├── ...
```



## CMakeLists:

顶层

```cmake
cmake_minimum_required(VERSION 2.8)

project("cub")

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof  -g -std=c++14")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory("src")

if(ENABLE_TEST)
  add_subdirectory(test)
endif()

install(DIRECTORY include/cub DESTINATION include)  #将头文件安装到/usr/local/include中
```

静态库libcub.a

```cmake
FILE(GLOB_RECURSE all_files
*.cpp
*.cc
*.c++
*.c
*.C)

add_library(cub STATIC ${all_files} ../include/cub/network/ByteOrder.h ../test/TestByteOrder.cpp)

install(TARGETS cub ARCHIVE DESTINATION lib)  #将libcub.a安装到/usr/local/lib中
# 头文件安装的部分也可以写在这里：install(DIRECTORY ../include/cub DESTINATION include)
```

单元测试：

```cmake
project(cub-test)

include_directories(${MAGELLAN_INCLUDE_DIR}) #可以不写，顶层cmakelists已经包含了

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

FILE(GLOB_RECURSE all_files
*.cpp
*.cc
*.c++
*.c
*.C)

add_executable(cub-test ${all_files})
target_link_libraries(cub-test cut cub)
```

只编译库文件：

```shell
cd cub
mkdir build
cd build
cmake ..
make
```

安装库文件：

```bash
sudo make install
```

生成单元测试可执行文件：

```bash
cd build
cmake -DENABLE_TEST=1 ..   #cmake -DENABLE_TEST=ON .. 也可以
make
./test/cub-test
```

# case5：

这个示例主要用来演示对源代码的条件编译。

源代码来自于：https://github.com/yanxicheung/CMakeNote/tree/main/CMakeDemo/case5

## 目录结构：

```bash
.
├── CMakeLists.txt
├── main.cpp
├── MathFunctions.cpp
└── MathFunctions.h
```

## CMakeLists:

```cmake
cmake_minimum_required (VERSION 2.8)
project (Demo2)

if(DEFINED USE_MYMATH)
    message("USE_MYMATH is defined")
    add_definitions(-DUSE_MYMATH) 
else()
    message("USE_MYMATH is not defined")
endif()

aux_source_directory(. DIR_SRCS)
add_executable(Demo ${DIR_SRCS})
```



# 参考文献：

1. CMake入门实战：https://www.hahack.com/codes/cmake/
2. https://github.com/wzpan/cmake-demo
3. CMake条件编译：https://www.cnblogs.com/binbinjx/p/5648957.html
4. CMake和Cpp的条件编译(一)：https://www.dennisthink.com/2020/10/18/877/
5. CMake和Cpp的条件编译(二)：https://www.dennisthink.com/2020/10/18/893/





