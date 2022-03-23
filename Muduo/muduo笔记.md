# 基础常识：

从线程和进程的角度来说，进程是资源分配的最小单位，线程是独立调度的最小单位。
同一个进程中的多个线程之间可以并发执行，他们共享进程资源。
线程不拥有资源，线程可以访问隶属进程的资源，**<u>进程有自己的独立空间地址，线程没有自己的独立空间地址，但是线程有自己的堆栈和局部变量</u>**。

线程的特点是共享地址空间，从而可以高效地共享数据。



# 线程标识：

 在linux下每一个进程都一个进程id，类型pid_t，可以由 **getpid（）**获取。

POSIX线程也有线程id，类型pthread_t，可以由 **pthread_self（）**获取，线程id由线程库维护。

但是各个进程独立，所以会有不同进程中线程号相同节的情况。

那么这样就会存在一个问题，我的进程p1中的线程pt1要与进程p2中的线程pt2通信怎么办，进程id不可以，线程id又可能重复，所以这里会有一个真实的线程id唯一标识tid。glibc没有实现gettid的函数，所以我们可以通过linux下的系统调用 **syscall(SYS_gettid)** 来获得。



# 在使用Multiplexed I/O的情况下，还有必要使用Non Blocking I/O么 ?

今天在看别人写的一段server代码的时候发现，他在使用select的情况下，仍然把注册到select里的file descriptor设置成了nonblocking。但是我的理解是，如果使用select的话，只会对ready的file descriptor操作，不会block啊。是我的理解有问题，还是他写的有问题？



题主的理解有问题。可以思考个简单的例子：

如果某个socket描述符返回可写，说明该描述符的发送缓冲区可用字节数高于低水位。那假设发送缓冲区低水位为100，目前发送缓冲区有1000Bytes的可用空间，如果你需要发2000Bytes的数据，如果socket是非阻塞的，发送完1000Bytes到发送缓冲区之后会立刻返回，等下次缓冲区可写时继续写；如果socket是阻塞的，发送完了1000Bytes到发送缓冲区，线程就阻塞了。

另外一个例子，前面陈硕的答案提到了Linux Programmer 's Manual，select的手册有如下内容：

> Under Linux, select() may report a socket file descriptor as "ready for reading", while nevertheless a subsequent read blocks.  This could for example happen  when  data has arrived but upon examination has wrong checksum and is discarded.  There may be other circumstances in which a file descriptor is spuriously reported as ready.  Thus it may be safer to use O_NONBLOCK on sockets that should not block.

当某个socket接收缓冲区有新数据分节到达，然后select报告这个socket描述符可读，但随后，协议栈检查到这个新分节检验和错误，然后丢弃这个分节，这时候调用read则无数据可读，如果socket没有被设置nonblocking，此read将阻塞当前线程。

可以看出，select返回某个描述符读写就绪，并不意味着接下来的读写操作全过程就一定不会阻塞。**所以I/O多路复用绝大部分时候是和非阻塞的socket联合使用。**



1.既然select都返回可读了，那就表示一定能读了

NO，select的可读是probably，不是certainly，当然不仅仅是select，所有的IO复用都是这样。

2.阻塞函数、非阻塞函数一样

NO，如果你去read一个不可读的非阻塞socket会立即返回EWOULDBLOCK，阻塞socket是不返回的。

3.有了select，阻塞和非阻塞函数一样

probably，大部分情况下没有差别，但是select可读异常情况下，非阻塞socket会立即返回EWOULDBLOCK

而且这个时候你肯定是读不到数据的，阻塞socket这个时候会一直阻塞。

https://www.cnblogs.com/moodlxs/archive/2011/09/15/2345385.html



# 学习笔记：

https://www.cnblogs.com/onlyandonly/p/13331140.html

https://www.cnblogs.com/ailumiyana/p/10087539.html

https://www.cnblogs.com/ailumiyana/category/1341869.html



# 类的构造、析构函数执行顺序

```cpp
struct A
{
	A()
	{
		cout << "A" << endl;
	}

	~A()
	{
		cout << "~A" << endl;
	}

};

struct B
{
	B()
	{
		cout << "B" << endl;
	}

	~B()
	{
		cout << "~B" << endl;
	}
};

struct C:A
{
	C()
	{
		cout << "C" << endl;
	}

	~C()
	{
		cout << "~C" << endl;
	}
private :
	B b;
};

int main()
{
	{
		C c;
	}
	system("pause");
	return 0;
}
```

执行结果：

```shell
A
B
C
~C
~B
~A
请按任意键继续. . .
```

构造时: 基类构造函数-->类成员构造函数-->类的构造函数

析构时: 类的析构函数-->类成员析构函数-->基类析构函数

调用到基类析构函数的时候，**<u>派生类对象的那部分已经析构</u>**



析构动作在创建时被捕获。这是一个非常有用的特性，这意味着：

� 虚析构不再是必须的。 

� shared_ptr 可以持有任何对象，而且能安全地释放

� shared_ptr 对象可以安全地跨越模块边界，比如从 DLL 里返回，而不会造成从模 块 A 分配的内存在模块 B 里被释放这种错误。 

� 二进制兼容性，即便 Foo 对象的大小变了，那么旧的客户代码任然可以使用新的 动态库，而无需重新编译（这要求 Foo 的头文件中不出现访问对象的成员的 inline 函数）。 

� 析构动作可以定制。



 shared_ptr 是管理共享资源的利器，需要注意避免循环引用，

通常的做 法是 owner 持有指向 A 的 shared_ptr，A 持有指向 owner 的 weak_ptr。



议尽量减少使用 跨线程的对象，用流水线，生产者-消费者，任务队列这些有规律的机 制，最低限度地共享数据。这是我所知最好的多线程编程的建议。

# shared_ptr技术：

定制析构功能

```cpp
template<class Y, class D> shared_ptr::shared_ptr(Y* p, D d);
template<class Y, class D> void shared_ptr::reset(Y* p, D d);
```



# shared_ptr线程安全性：

在多线程中，同时访问同一个`shared_ptr`对象，需要加锁保护。



# shared_ptr计数与陷阱：

1、意外延长的生命周期。

由于shared_ptr是强引用，只要有一个指向对象x的shared_ptr存在，x就不会析构；这也是java常见的内存泄漏原因。

另外这里还提到了boost:bind，也就是std::bind，会把实参中的shared_ptr拷贝一份，导致该对象生命周期不会短于function对象。这里可能会在不经意间延长对象生命周期。

```cpp
struct Foo
{
	Foo()
	{
		cout << "Foo" << endl;
	}

	~Foo()
	{
		cout << "~Foo" << endl;
	}

	void doit()
	{

	}
};

std::function<void()>  gfunc;
int main()
{
	{
		shared_ptr<Foo> pFoo(new Foo);
		cout << "count:"<<pFoo.use_count() << endl;  // use_count 1
		gfunc = std::bind(&Foo::doit, pFoo); // 持有对pFoo的一份拷贝,会延长pFoo指向对象的生命周期;
		cout << "count:" << pFoo.use_count() << endl; // use_count 2
	}
	system("pause");
	return 0;
}

```

2、函数参数。

由于要修改引用计数，shared_ptr的拷贝开销比原始指针高。不过只需要确保最外层函数有一个实体对象，之后都可以用const reference的方式来使用这个shared_ptr，无需拷贝。

3、析构所在的线程。

shared_ptr<void> 可以持有任何对象

对象的析构是同步的，当最后一个指向对象x的shared_ptr离开其作用域时，x会同时在同一个线程析构。如果析构比较耗时，那可能会拖慢关键线程的性能，因此有必要的话可以用一个单独线程专门做析构。；**同时，我们可以用一个单独的线程来专门做析构 ，通过一个 BlockingQueue<shared_ptr<void> > 把对象的析构都转移到那个专用线程，从而解放关键线程。**---具体怎么搞？？

4、现成的RAII handle

RAII 资源获取即初始化，初学C++会告诉我们，new了之后要delete。如果使用RAII，每一个new出来的资源都应该立刻交由handle管理（如shared_ptr），一般不去delete。shared_ptr要避免循环使用，通常做法时owner持有child的shared_ptr，child持有owner的weak_ptr。



高性能网络编程经典：The C10K problem



如果一个函数既可能在已加锁的情况下调用，又可能在未加锁的情况下调用，那么就拆 成两个函数： 1. 跟原来的函数同名，函数加锁，转而调用第 2 个函数。 2. 给函数名加上后缀 WithLockHold，不加锁，把原来的函数体搬过来。

```cpp
void post(const Foo& f)
{
    MutexLockGuard lock(mutex);
    postWithLockHold(f); // 不用担心开销，编译器会自动内联的
}
// 引入这个函数是为了体现代码作者的意图，尽管 push_back 通常可以手动内联
void postWithLockHold(const Foo& f)
{
    foos.push_back(f);
}
```



# 空悬指针、野指针、内存泄露：

空悬指针：指向已经销毁的对象或者已经回收的地址。

野指针：未初始化的指针。

内存泄露：已动态分配的堆内存由于某种原因程序未释放或无法释放，造成系统内存的浪费，导致程序运行速度减慢甚至系统崩溃等严重后果。



# 线程：

线程的特点是共享地址空间，从而可以高效地共享数据。

一台机器上的多个进程能高效地共享代码段（操作系统可以映射为同样的物理内存），但不能共享数据。

如果多个进程大量共享内存，等于是把多进程程序当成多线程来写，掩耳盗铃。

“多线程”的价值，我认为是为了更好地发挥对称多路处理 (SMP) 的效能。在 SMP 之前，多线程没有多大价值。Alan Cox 说过 A computer is a state machine. Threads are for people who can't program state machines. （计算机是一台状态机。线程是给那些不能编写状态机程序的人准备的。）如果只有一个执行单元，一个 CPU，那么确实如 Alan Cox 所说，按状态机的思路去写程序是最高效的。





在高性能的网络程序中，使用得最为广泛的恐怕要数“nonblocking IO + IO multiplexing”这种模型，即 Reactor 模式

在“non-blocking IO + IO multiplexing”这种模型下，程序的基本结构是一个事件循环 (event loop)：（代码仅为示意，没有完整考虑各种情况）

```cpp
while (!done)
{
	int timeout_ms = max(1000, getNextTimedCallback());
	int retval = ::poll(fds, nfds, timeout_ms);
    if (retval < 0) 
    {
    	处理错误
	} 
    else 
    {
		处理到期的 timers
		if (retval > 0) 
        {
			处理 IO 事件
		}
}
```

Reactor 模型的优点很明显，编程简单，效率也不错。

不仅网络读写可以用，连接的建 立（connect/accept）甚至 DNS 解析都可以用非阻塞方式进行，以提高并发度和吞吐量 (throughput)。

对于 IO 密集的应用是个不错的选择。



# non-blocking IO + one loop per thread

线程池，用 blocking queue 实现的任务队列(TaskQueue)：

总结起来，我推荐的多线程服务端编程模式为：event loop per thread + thread pool。

 � event loop 用作 non-blocking IO 和定时器。 

� thread pool 用来做计算，具体可以是任务队列或消费者-生产者队列。 

以这种方式写服务器程序，需要一个优质的基于 Reactor 模式的网络库来支撑。



# 进程间通信

进程间通信我首选 Sockets（主要指 TCP，我没有用过 UDP，也不考虑 Unix domain 协议），其最大的好处在于：可以跨主机，具有伸缩性。反正都是多进程了，如果一台机器 处理能力不够，很自然地就能用多台机器来处理。把进程分散到同一局域网的多台机器上， 程序改改 host:port 配置就能继续用。相反，前面列出的其他 IPC 都不能跨机器（比如共 享内存效率最高，但再怎么着也不能高效地共享两台机器的内存），限制了 scalability。



# 线程间同步

线程同步的四项原则，按重要性排列： 

1. 首要原则是尽量最低限度地共享对象，减少需要同步的场合。一个对象能不暴露给 别的线程就不要暴露；如果要暴露，优先考虑 immutable 对象；实在不行才暴露 可修改的对象，并用同步措施来充分保护它。
2. 其次是使用高级的并发编程构件，如 TaskQueue、Producer-Consumer Queue、 CountDownLatch 等等； 
3. 最后不得已必须使用底层同步原语 (primitives) 时，只用非递归的互斥器和条件 变量，偶尔用一用读写锁；
4. 不自己编写 lock-free 代码，不去凭空猜测“哪种做法性能会更好”，比如 spin lock vs. mutex。



互斥器 (mutex) 恐怕是使用得最多的同步原语，粗略地说，它保护了临界区，一个时 刻最多只能有一个线程在临界区内活动。（请注意，我谈的是 pthreads 里的 mutex，不是 Windows 里的重量级跨进程 Mutex。）单独使用 mutex 时，我们主要为了保护共享数据。 我个人的原则是： 

� 用 RAII 手法封装 mutex 的创建、销毁、加锁、解锁这四个操作。 

� 只用非递归的 mutex（即不可重入的 mutex）。

� 不手工调用 lock() 和 unlock() 函数，一切交给栈上的 Guard 对象的构造和析构 函数负责，Guard 对象的生命期正好等于临界区（分析对象在什么时候析构是 C++ 程序员的基本功）。这样我们保证在同一个函数里加锁和解锁，避免在 foo() 里加 锁，然后跑到 bar() 里解锁。 

� 在每次构造 Guard 对象的时候，思考一路上（调用栈上）已经持有的锁，防止因 加锁顺序不同而导致死锁 (deadlock)。由于 Guard 对象是栈上对象，看函数调用 栈就能分析用锁的情况，非常便利。

 次要原则有： 

� 不使用跨进程的 mutex，进程间通信只用 TCP sockets。 

� 加锁解锁在同一个线程，线程 a 不能去 unlock 线程 b 已经锁住的 mutex。（ RAII 自动保证） 

� 别忘了解锁。（RAII 自动保证） 

� 不重复解锁。（RAII 自动保证） 

� 必要的时候可以考虑用 PTHREAD_MUTEX_ERRORCHECK 来排错



# one loop per thread:

文中的“多线程服务器”是指运行在 Linux 操作系统上的独占式网络应用程序。硬件

平台为 Intel x64 系列的多核 CPU，单路或双路 SMP 服务器（每台机器一共拥有四个核或

八个核，十几 GB 内存），机器之间用百兆或千兆以太网连接。这大概是目前民用 PC 服务

器的主流配置。



单线程服务器编程模型：

1. non-blocking IO + IO multiplexing

多线程服务器编程模型：

1. 每个请求创建一个线程，使用阻塞式 IO 操作
2. thread pool，同样使用阻塞式 IO 操作
3. 使用 non-blocking IO + IO multiplexing



推荐的多线程服务端编程模式为：event loop per thread + thread pool。

1. event loop 用作 IO multiplexing，配合non-blocking IO 和定时器。
2. thread pool用来做计算，具体可以是任务队列或者生产者消费者队列。





BlockingQueue 

任务队列   

数据的生产者消费者队列

![image-20220125084836489](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220125084836489.png)

多线程不能提高绝对性能，但是能够提高平均响应性能。



[ulimit -a查看线程栈大小](https://blog.csdn.net/liyuanyes/article/details/44097731)



`pthread_t`不适合用作程序中对线程的标识符，应该使用`gettid`作为线程的id，0是非法值。



`__thread pid_t t_cachedTid = 0;`

凡是带有__thread的变量，每个线程都拥有该变量的一份拷贝，且互不干扰。线程局部存储中的变量将一直存在，直至线程终止，当线程终止时会自动释放这一存储。__thread并不是所有数据类型都可以使用的，因为其只支持POD

- **有时需要提供线程私有数据：可以跨多个函数访问(全局)；仅在某个线程有效(私有)(即在线程里面是全局）。例如：errno。**



 只能修饰**POD**类型(类似整型指针的标量，不带自定义的构造、拷贝、赋值、析构的类型，二进制内容可以任意复制memset,memcpy,且内容可以复原)
  不能修饰class类型，因为无法自动调用构造函数和析构函数，可以用于修饰全局变量，函数内的静态变量，不能修饰函数的局部变量或者class的普通成员变量，且__thread变量值只能初始化为编译期常量，即编译期间就能确定值。

[thread关键字](https://www.cnblogs.com/codestack/p/13395797.html)



```cpp
void threadFunc()
{
   static  int a = 0;   //static变量每个线程看到的a是同一个   
   // __thread  static  int a = 0;  //每个线程看到的a不是同一个
    //   int a = 0;    // 每个线程看到的a不是同一个
  a++;
  printf("threadFunc tid=%d, &a[%p],a[%d]\n", muduo::CurrentThread::tid(),&a,a);
}
```

线程是稀缺资源，一个进程可以创建的并发线程数目受限于地址空间大小和内核参数，一台机器可以同时并行运行的线程数目受限于CPU的数目。





ch4.6多线程与IO好好看看；



![image-20220125135757665](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220125135757665.png)











# 条件变量：

上学的时候看到pthread_cond就觉得有点复杂，学了半天，而且这玩意经常伴随消费生产模型出现，更让人迷迷糊糊。工作了以后用的不多，大部分场景下加个锁就够了，今天突然看到有个go项目中用了cond，仔细琢磨了下cond到底该怎么用，什么时候用：

条件变量（cond）和锁（mutex）是紧密相关的，锁的使用场景是：这件事同时只有一个人能做，我抢到锁就进去做了，我做完再给下一个人做。这时就加个锁，保证某些变量同时只被一个线程操作。
什么情况下要用到条件变量呢，首先这件事还是只能一个人做，所以还要用锁。但线程抢到锁了后，发现还要等待一些条件满足，才能做。
这时怎么办呢？难道抢到锁的线程要不停检查这个条件吗？消耗高不说，每次检查完总得sleep一下吧，不然cpu都空跑了，sleep的时间怎么定？sleep少了，消耗高；多了，又有延迟，不能即时发现条件满足了。而且你抢到锁一直检查，别人也拿不到锁了。
这时就引入了条件变量，抢到锁的线程发现条件未满足时，释放锁，使用pthread_cond_wait()挂起自己；这时别的线程也能获取锁进来，发现条件不满足同样挂起；直到条件满足后，由其他方调用pthread_cond_broacast（唤醒全部挂起的线程）或者pthread_cond_signal（唤醒一个线程），通知这些挂起的线程说：条件好了，快来做吧，这时，第一个抢到锁的线程进去就可以干活了。

为什么提到cond经常会提到消费者生产者模型呢？
因为生产者消费者模型正好满足这种情况：1. 同一时间事情只能一人来做：生产者消费者都要操作同一个队列（或者其他容器），容器需要桶保护 2. 要满足条件才能干事：需要调生产出东西来，才能消费。所以消费方是pthread_cond_wait等待的那一方，生产者是pthread_cond_signal通知的那一方。

<u>**总结起来就是效率高。**</u>



条件变量 (condition variable) 顾名思义是一个或多个线程等待某个布尔表达式为真， 即等待别的线程“唤醒”它。条件变量的学名叫管程 (monitor)。Java Object 内置的 wait(), notify(), notifyAll() 即是条件变量（它们以容易用错著称）。条件变量只有一种正确使用的 方式，对于 wait() 端：

1. 必须与 mutex 一起使用，该布尔表达式的读写需受此 mutex 保护 
2. 在 mutex 已上锁的时候才能调用 wait() 
3. 把判断布尔条件和 wait() 放到 while 循环中

```cpp
MutexLock mutex;
Condition cond(mutex);
std::deque<int> queue;
int dequeue()
{
    MutexLockGuard lock(mutex);
    while (queue.empty()) 
    { // 必须用循环；必须在判断之后再 wait()
   	 	cond.wait(); // 这一步会原子地 unlock mutex 并进入 blocking，不会与 enqueue 死锁
    }
    assert(!queue.empty());
    int top = queue.front();
    queue.pop_front();
    return top;
}
```

对于 signal/broadcast 端： 

1. 不一定要在 mutex 已上锁的情况下调用 signal （理论上） 
2. 在 signal 之前一般要修改布尔表达式 
3. 修改布尔表达式通常要用 mutex 保护（至少用作 full memory barrier）

```cpp
void enqueue(int x)
{
    MutexLockGuard lock(mutex);
    queue.push_back(x);
    cond.notify();
}
```

上面的 dequeue/enqueue 实际上实现了一个简单的 unbounded BlockingQueue。

条件变量是非常底层的同步原语，很少直接使用，一般都是用它来实现高层的同步措施 ， 如 BlockingQueue 或 CountDownLatch。



# 感悟、总结：

提供灵活性固然是本事，然而在不需要灵活性的地方把代码写死，更需要大智慧。

掌握多线程编程，才能更理智地选择用还是不用多 线程，因为你能预估多线程实现的难度与收益，在一开始做出正确的选择。要知道把一个单 线程程序改成多线程的，往往比重头实现一个多线程的程序更难。



等待一个 事件发生，正确的做法是用 select 或 condition variable 或（更理想地）高层同步工具。



# 代码片段：

## 格式化打印：

```cpp
char buff[1000] = {0};
sprintf(buff, "hello\r\n"
        "world\r\n");
```

## 删除vector中元素：

通过交换方式删除：

```cpp
 std::vector<Observer*>::iterator it = std::find(observers_.begin(), observers_.end(), x);
  if (it != observers_.end())
  {
    std::swap(*it, observers_.back());
    observers_.pop_back();
  }
```



## enable_shared_from_this：

https://blog.csdn.net/caoshangpa/article/details/79392878

为了使用enable_shared_from_this(),对象不能是stack object，必须是heap object，并且由shared_ptr来管理其生命周期。

enable_shared_from_this(),不能在构造函数中调用,因为在构造的时候，对象还没有被shared_ptr接管;

## 关于vector:

vector析构时会调用对象的析构函数；

如果对象中分配了动态资源，则对象**需要定义拷贝构造函数**，进行深拷贝，否则资源会释放两次，导致程序崩溃。

```cpp
struct test{
	test(){
		pInt = new int(100);
		cout << "test()" << endl;
	}
	~test(){
		cout << "~test()"<<this<<" "<<*pInt << endl;
		delete pInt;
	}

	test(const test& rhs)
	{
		if (this == &rhs)
			return;
		pInt = new int(*rhs.pInt);
		cout << "test(const test& rhs)" << endl;
	}


	int a;
	int *pInt;

};
int main()
{
	{
		test ddd;
		vector<test> vt;
		vt.push_back(ddd);
	}
	system("pause");
	return 0;
}
```

## 赋值运算符题目：

```cpp

class CMyString
{
public:
	CMyString(char* pData = nullptr);
	CMyString(const CMyString& str);
	~CMyString(void);
	CMyString& operator = (const CMyString& str);
private:
	char* m_pData;
};

CMyString::CMyString(char* pData /*= nullptr*/)
{
	if (pData == NULL)
	{
		m_pData = new char[1];
		m_pData[0] = '\0';
	}
	else
	{
		int len = strlen(pData);
		m_pData = new char[len + 1];
		strcpy(m_pData, pData);
	}
}

CMyString::CMyString(const CMyString& str)
{
	int len = strlen(str.m_pData);
	m_pData = new char[len + 1];
	strcpy(m_pData, str.m_pData);
}

CMyString::~CMyString(void)
{
	delete[]m_pData;
}

CMyString& CMyString::operator=(const CMyString& str)
{
	if (this == &str)
		return *this;

	CMyString tmp(str); // 调用拷贝构造;
	char *ptemp = tmp.m_pData;
	tmp.m_pData = m_pData;
	m_pData = ptemp;
	// 自动调用析构释放以前的内存;（原对象）

	return *this;
}
```

