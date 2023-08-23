# S00

`__thread`  线程局部变量，利用线程局部变量可以知道某个对象的创建和调用是否在同一个线程。



# S01

每个Channel对象都只属于某一个IO线程，每个Channel对象自始自终只负责一个fd的IO事件分发，但是它不拥有这个fd，也不会在析构的时候关闭这个fd。



Poller是EventLoop的间接成员，Poller不拥有Channel，Channel在析构前必须自己解注册(EventLoop::removeChannel)，避免悬空指针。



如果某个Channel 暂时不关心任何事件，就将pollfd.fd 设为-1，让poll 忽略此项。
这里可以进一步改进为： 将pollfd.fd 设为 channel->fd() 的相反数 减1 ，这样可以进一步检查invariant。
考虑为什么是 -1？ 因为文件描述符0、1、2分别表示标准输入、标准输出、标准错误。-1、-2、-3 可分别表示忽略 标准输入、标准输出、标准错误。



poll(2)是level trigger，不读取数据下次会立即触发。

```cpp
#include "Channel.h"
#include "EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>

muduo::EventLoop* g_loop;
int timerfd;
void timeout()
{
  printf("Timeout!\n");
//  g_loop->quit();
//  poll 为边沿触发,不读取数据下次会立即触发;会狂打Timeout
//  uint64_t howmany;
//  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
}

int main()
{
  muduo::EventLoop loop;
  g_loop = &loop;

  timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  muduo::Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 2;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);

  loop.loop();

  ::close(timerfd);
}

```

# S02

## TimerQueue

使用`int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);`

目前为止TimerQueue::addTimer接口只能在IO线程中被调用。

set容器的key就是value，如果key是用户自定义类型，自定义<运算符。

std::pair 是可以比较大小的，有这样的全局函数：

```cpp
template<class _Ty1,
	class _Ty2> inline
	bool operator<(const pair<_Ty1, _Ty2>& _Left,
		const pair<_Ty1, _Ty2>& _Right)
	{	// test if _Left < _Right for pairs
	return (_Left.first < _Right.first ||
		!(_Right.first < _Left.first) && _Left.second < _Right.second);
	}
```

也就是说可以比较大小，先按first比较，如果相等，再按照second比较。



RVO是一种编译器优化技术，可以把通过函数返回创建的临时对象给”去掉”，然后可以达到少调用拷贝构造的操作目的，它是C++11标准的一部分。



# S03

EventLoop::runInLoop在IO线程内执行用户的回调任务。

支持TimerQueue::addTimer接口在其他线程中被调用。【使用EventLoop::runInLoop】

使用`eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);`对IO线程进行唤醒。

EventLoop::queueInLoop可以在IO线程中调用，也可以在其他线程中调用。



EventLoopThread Class   ---> IO 线程不一定是主线程，EventLoopThread 让我们可以在任何一个线程创建并运行EventLoop



# S04

Acceptor：用于新TCP连接，并通过回调通知使用者【谁使用Acceptor提供的设置回调函数的接口，谁就是使用者】，他是内部class ，供TcpServer使用，生命周期由TcpServer控制。

```cpp
void newConnection(int sockfd, const muduo::InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
  muduo::sockets::close(sockfd);
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  muduo::EventLoop loop;

  muduo::InetAddress listenAddr(9981);
  muduo::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();

  loop.loop();
}

```



# S05

实现TcpServer，初步实现TcpConnection

TcpConnection是最核心，也是最复杂的class，使用shared_ptr来管理，也是唯一继承enable_shared_from_this的class，这源于其模糊的生命周期.

> 在非阻塞网络编程中，我们常常要面临这样一种场景：
> 从某个TCP 连接A收到了一个request，程序开始处理这个request；处理可能要花一 定的时间，为了避免耽误（阻塞）处理其他request，程序记住了发来request的TCP连接，在某个线程池中处理这个请求。在处理完之后，会把response发回TCP连接A
> 但是，在处理request的过程中，客户端断开了TCP连接A，而另一个客户端刚好创建了新连接B。
>
> 我们的程序不能只记住TCP连接A的文件描述符，而应该持有封装socket连接的TcpConnection对象，保证在处理request期间TCP连接A的文件描述符不会被关闭。
>
> 或者持有TcpConnection对象的弱引用（weak_ptr），这样能知道socket连接在处理request期间是否已经关闭了，fd＝8的文件描述符到底是“前世”还是“今生”。
> 否则的话，旧的TCP连接A一断开，TcpConnection对象销毁，关闭了旧的文件描述符（RAII），而且新连接B的socket文件描述符有可能等于之前断开的TCP连接（这是完全可能的，POSIX要求每次新建文件描述符时选取当前最小的可用的整数）。当程序处理完旧连接的request时，就有可能把response发给新的TCP连接B，造成串话
>
> 为了应对这种情况，防止访问失效的对象或者发生网络串话， muduo使用shared_ptr来管理TcpConnection的生命期。这是唯一一个采用引用计数方式管理生命期的对象。如果不用shared_ptr，我想不出其他安全且高效的办法来管理多线程网络服务端程序中的并发连接 

TcpConnection拥有TCP Socket他的析构函数会close(fd)【没看到代码这么处理】

目前的问题：客户端断开连接后，服务端会陷入busy loop

# S06

主要介绍TCP断开的处理。

```cpp
void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::handleClose state = " << state_;
  assert(state_ == kConnected);
  // we don't close fd, leave it to dtor, so we can find leaks easily.
  channel_->disableAll();
  // must be the last line
  LOG_INFO << "handleClose use_count "<<shared_from_this().use_count(); // 2
//  closeCallback_(shared_from_this());

  shared_from_this().use_count();  //不会怎加引用计数
 // shared_from_this();            // 不会怎加引用计数
 // const TcpConnectionPtr& p =  shared_from_this();  // 也会增加引用计数  2   延长了生命周期;
  TcpConnectionPtr p =  shared_from_this();  // 也会增加引用计数  2
  closeCallback_(NULL);
}
```



`shared_from_this`的原型，返回一个临时变量， const TcpConnectionPtr& p 延长了类的生命周期，所以引用计数加1

```cpp
shared_ptr<_Ty> shared_from_this()
{	// return shared_ptr
	return (shared_ptr<_Ty>(_Wptr));
}

shared_ptr<const _Ty> shared_from_this() const
{	// return shared_ptr
	return (shared_ptr<const _Ty>(_Wptr));
}
```



# S07

## Buffer

[Muduo 设计与实现之一：Buffer 类的设计](https://www.cnblogs.com/Solstice/archive/2011/04/17/2018801.html)

Buffer的读取



# S08

介绍TCP发送数据



# S09

`SIGPIPE`的默认行为是终止进程，在网络编程中，这意味着如果对方断开连接而本地继续写入的话，会造成服务进程意外退出。

```cpp
class IgnoreSigPipe
{
 public:
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE, SIG_IGN);
  }
};

IgnoreSigPipe initObj;
```



# S10

多线程TcpServer

# S11

实现Connector

Connector只负责建立socket连接，不负责创建`TcpConnection`

socket 是一次性的，一旦出错就无法恢复，只能关闭重来。

客户端的socket变得可写时，表明连接建立完毕，但是也不意味着连接成功建立，还需要使用

`getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen)`再次确认

如果本地TCP程序本地通信，且客户端先于服务端启动，那么有极大可能会产生一种自连接现象。



# S12

实现TcpClient



# S13





# 测试

```cpp
#include <iostream>
#include <string>
#include <exception>
#include <functional>
#include <memory>
using namespace std;
using namespace std::placeholders;

typedef function<void(void)> cb;

class World
{
public:
	World()
	{
	}
	void setcb(cb xxxcb)
	{
		m_cb = xxxcb;
	}
private:
	cb m_cb;
};

class Hello:public enable_shared_from_this<Hello>
{
public:
	Hello()
	{

	}
	~Hello()
	{
		cout << "=========" << endl;
	}
	void Init()
	{
		m_world.setcb(std::bind(&Hello::Zpp, shared_from_this())); // 将shared_from_this改为this可以被销毁。
	}
private:
	void Zpp()
	{

	}
private:
	World m_world;
};

int main(int argc, char** argv)
{
	{
		shared_ptr<Hello>  pHello(new Hello);
		pHello->Init();
	}  // 对象Hello不会被销毁，
	system("pause");
	return 0;
}

```



```cpp
int main(int argc, char** argv)
{
	vector<char>   ddd(100);
	cout << ddd.size() << endl;     // 100
	cout << ddd.capacity() << endl; // 100

	ddd.push_back('c');
	cout << ddd.size() << endl;     // 101
	cout << ddd.capacity() << endl; // 150

	ddd.resize(50);
	cout << ddd.size() << endl;     // 50
	cout << ddd.capacity() << endl; // 150
	system("pause");
	return 0;
}
```

