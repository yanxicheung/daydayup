![Supported Platforms](https://img.shields.io/badge/platform-Linux-red.svg)
[![Build Status](https://travis-ci.org/downdemo/IO-Multiplexing.svg?branch=master)](https://travis-ci.org/downdemo/IO-Multiplexing)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/downdemo/IO-Multiplexing/blob/master/LICENSE)

## Build and Run

* build

```
git clone git@github.com:downdemo/IO-Multiplexing.git
cd IO-Multiplexing
make
```

* Service would be generated in build directory

```
cd build
```

* Run server and client in different shell is recommended
* Run server before client
* One running server at most because samples use the same port
* `Ctrl + c` to interrupt blocking server and client process

### Run [socket sample](/socket)

```
./socket_server&./socket_client
```

### Run [fork sample](/fork)

* Run server

```
./fork_server
```

* Run client multiple times

```
./fork_client
./fork_client
./fork_client
./fork_client
./fork_client
```

### Run [select sample](/select)、[poll sample](/poll)、[epoll sample](/epoll)

* Run any I/O multiplexing server, such as epoll_server

```
./epoll_server
```

* Run multiple I/O multiplexing clients

```
./select_client
./select_client
./poll_client
./poll_client
./epoll_client
./epoll_client
```

* TCP connection status

```
netstat -antp | grep 12345
```

## 五种 I/O 模型

* 一个输入操作通常包括两个阶段
  * 等待数据准备好，从网络中到达时复制到内核的某个缓冲区
  * 从内核缓冲区向进程缓冲区复制数据
* 同步 I/O 操作的四种模型都会导致请求进程阻塞，直到 I/O 操作完成，它们第二阶段都一样，主要区别在第一阶段
  * blocking I/O（阻塞 I/O）：最常见的模型，Linux 中默认所有 socket 都是阻塞的。进程调用 recvfrom，内核等待数据到达，用户线程阻塞，内核接收到数据后，将其拷贝到进程缓冲区，并返回结果，此时用户才解除阻塞状态。整个 I/O 请求过程中，用户线程都是阻塞的，不能做任何事，CPU 资源利用率低
  * non-blocking I/O（非阻塞 I/O）：即轮询内核。进程调用 recvfrom 时没有数据返回，内核不阻塞用户，而是返回一个 EWOULDBLOCK 错误，虽然没读取任何数据，但用户能马上得到返回结果。用户收到错误码，知道内核还没准备好，会不断发送请求直到内核准备好数据，此时内核再把数据拷贝到进程缓冲区然后返回。轮询会消耗大量 CPU 资源，一般很少直接用非阻塞 I/O，而是在其他 I/O 模型中使用这一特性
  * I/O multiplexing（I/O 多路复用）：最常用的 I/O 模型，可以理解为某些进程需要的一种预先告知内核的能力，使得内核一旦发现进程指定的一个或多个 I/O 条件就绪（即输入已准备好被读取，或描述符已能承接更多的输出），它就通知进程。I/O 多路复用建立在内核提供的 select 函数的基础上，用于避免非阻塞 I/O 的轮询等待问题。用户先将要进行 I/O 操作的文件描述符放到 select 中，select 会阻塞，直到当文件描述符可读时返回。select 的优势在于可以直接用单线程监听多个文件描述符，如果用阻塞 I/O 则需要多线程才能实现此目的。 select 会阻塞线程（但不阻塞 socket），所以 I/O 多路复用模型也叫异步阻塞 I/O 模型，但不算真正的异步 I/O
  * signal-driven I/O（信号驱动式 I/O）：通过 sigaction 函数安装信号处理函数，如果数据不可用，进程继续工作而不会阻塞，当数据准备好时，内核为该进程产生一个 SIGIO 信号，随后即可以在信号处理函数中调用 recvfrom 读取数据，并通知主循环数据已准备好待处理，也可以立即通知主循环让它读取数据报。这种模型的优势在于等待数据到达期间不被阻塞
* 异步 I/O 操作
  * asynchronous I/O（异步 I/O）：通知内核启动某个操作，内核在完成操作后通知用户。信号驱动模型是内核通知用户可以启动 I/O 操作的时机，而异步 I/O 是通知完成 I/O 操作的时机。异步 I/O 相比于 I/O 多路复用并不常用，I/O 多路复用加上多线程任务处理架构能满足大多数高性能并发程序要求
* 同步异步的区别：同步发出调用时，没得到结果就不返回，异步 I/O 发出调用不会马上得到返回结果。简单来说，同步 I/O 必须做完一件事才会做下一件事，会阻塞线程的 I/O 操作就是同步 I/O，因此前四种都是同步 I/O

## select

* 调用 select 函数会让内核轮询 fd_set，直到某个 fd_set 有就绪的文件描述时返回，内核标记该文件描述符所在的位，未就绪的位均置零。传入的第一个参数表示内核对 fd_set 轮询的位数，轮询范围应该为 0 到最大的文件描述符，因此第一个参数一般设置为最大文件描述符加 1

```cpp
#include <sys/select.h>
#include <sys/time.h>

/* Check the first NFDS descriptors each in READFDS (if not NULL) for read
   readiness, in WRITEFDS (if not NULL) for write readiness, and in EXCEPTFDS
   (if not NULL) for exceptional conditions.  If TIMEOUT is not NULL, time out
   after waiting the interval specified therein.  Returns the number of ready
   descriptors, or -1 for errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */

// 文件描述符就绪则返回其数目，超时返回 0，出错返回 -1
extern int select (int __nfds, fd_set *__restrict __readfds,
           fd_set *__restrict __writefds,
           fd_set *__restrict __exceptfds,
           struct timeval *__restrict __timeout);
```

* 如果设置了超时时间，即使没有文件描述符就绪，也会在超时时间返回。虽然 timeval 可以设置到微秒级，但内核的实际支持往往粗糙得多，一般 Unix 内核会往 10 ms 的倍数向上取整

```cpp
struct timeval
{
  __time_t tv_sec;        /* Seconds.  */
  __suseconds_t tv_usec;    /* Microseconds.  */
};
```

* fd_set 的本质是一个 long int 数组

```cpp
/* The fd_set member is required to be an array of longs.  */
typedef long int __fd_mask;

/* Some versions of <linux/posix_types.h> define this macros.  */
#undef    __NFDBITS
/* It's easier to assume 8-bit bytes than to get CHAR_BIT.  */
#define __NFDBITS    (8 * (int) sizeof (__fd_mask))
#define    __FD_ELT(d)    ((d) / __NFDBITS)
#define    __FD_MASK(d)    ((__fd_mask) (1UL << ((d) % __NFDBITS)))

/* fd_set for select and pselect.  */
typedef struct
  {
    /* XPG4.2 requires this member name.  Otherwise avoid the name
       from the global namespace.  */
#ifdef __USE_XOPEN
    __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->fds_bits)
#else
    __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->__fds_bits)
#endif
  } fd_set;
```

* 大小为 1024 bit

```cpp
/* Number of descriptors that can fit in an `fd_set'.  */
#define __FD_SETSIZE        1024
```

* 因此 fd_set 也相当于一个 bit_map，FD_SET 函数根据文件描述符的值，将对应的 fd_set 位置 1

```cpp
void FD_SET(int fd, fd_set *fdset);
```

* 当 select 返回时，仅表示有文件描述符就绪，但不知道是哪一个，因此还需要再次遍历所有文件描述符做 FD_ISSET 检查。另外，select 会修改传入的参数，因此下一次调用 select 时仍要重新设置 fd_set 和 timeout（timeout 会被清零，如果不重新设置，没有描述符就绪就会直接超时）。可以通过如下四个函数操作 fd_set，其实现本质是位运算，比如 FD_SET 先除以数组每个元素的大小找出新的 fd 所在的下标，再将 1 左移余数数量位的结果与该元素求或

```cpp
/* Access macros for `fd_set'.  */
#define    FD_SET(fd, fdsetp)    __FD_SET (fd, fdsetp)
#define    FD_CLR(fd, fdsetp)    __FD_CLR (fd, fdsetp)
#define    FD_ISSET(fd, fdsetp)    __FD_ISSET (fd, fdsetp)
#define    FD_ZERO(fdsetp)        __FD_ZERO (fdsetp)

#define __FD_SET(d, set) \
  ((void) (__FDS_BITS (set)[__FD_ELT (d)] |= __FD_MASK (d)))
#define __FD_CLR(d, set) \
  ((void) (__FDS_BITS (set)[__FD_ELT (d)] &= ~__FD_MASK (d)))
#define __FD_ISSET(d, set) \
  ((__FDS_BITS (set)[__FD_ELT (d)] & __FD_MASK (d)) != 0)
# define __FD_ZERO(fdsp) \
  do {                                                \
    int __d0, __d1;                                   \
    __asm__ __volatile__ ("cld; rep; " __FD_ZERO_STOS \
              : "=c" (__d0), "=D" (__d1)              \
              : "a" (0), "0" (sizeof (fd_set)         \
                      / sizeof (__fd_mask)),          \
                "1" (&__FDS_BITS (fdsp)[0])           \
              : "memory");                            \
  } while (0)

#else    /* ! GNU CC */

#define __NFDBITS    (8 * (int) sizeof (__fd_mask))
#define    __FD_ELT(d)    ((d) / __NFDBITS)
#define    __FD_MASK(d)    ((__fd_mask) (1UL << ((d) % __NFDBITS)))
```

* 通常一个 select 函数的调用过程如下

```cpp
// 准备多个 fd，保存到一个 fds 数组中，最大的记为 max_fd
fd_set read_fds;
while (true) {
  FD_ZERO(&read_fds);
  FD_SET(fds[0], &read_fds);
  FD_SET(fds[1], &read_fds);
  FD_SET(fds[2], &read_fds);
  timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  int ret = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
  if (ret == -1) {
    std::cout << "errno: " << strerror(errno) << '\n';
    break;
  }
  if (ret == 0) {
    std::cout << "timeout" << '\n';
    continue;
  }
  for (int i = 0; i < fd_count; ++i) {
    if (FD_ISSET(fds[i], &read_fds)) {
      ...
    }
  }
}
```

* select 的本质是中断，CPU 在每个指令周期结束后检查中断寄存器，如果发生中断则保存当前上下文再转而处理中断。中断本质是硬件轮询，比起直接用代码轮询（软件层面的轮询，本质是通过跳转指令实现），CPU 效率更高，时间成本几乎为 0

## poll

* poll 的原理和 select 相同，只不过参数类型有一些区别，poll 轮询的数据结构是元素类型为 pollfd 的数组（因此监听的文件描述符不仅限于 1024 个），nfds 是元素个数，timeout 是一个表示毫秒的整型

```cpp
#include <poll.h>

int poll (struct pollfd *__fds, nfds_t __nfds, int __timeout);

struct pollfd
  {
    int fd;            /* File descriptor to poll.  */
    short int events;        /* Types of events poller cares about.  */
    short int revents;        /* Types of events that actually occurred.  */
  };

/* Event types that can be polled for.  These bits may be set in `events'
   to indicate the interesting event types; they will appear in `revents'
   to indicate the status of the file descriptor.  */
#define POLLIN         0x001        /* There is data to read.  */
#define POLLPRI        0x002        /* There is urgent data to read.  */
#define POLLOUT        0x004        /* Writing now will not block.  */

#if defined __USE_XOPEN || defined __USE_XOPEN2K8
/* These values are defined in XPG4.2.  */
# define POLLRDNORM    0x040        /* Normal data may be read.  */
# define POLLRDBAND    0x080        /* Priority data may be read.  */
# define POLLWRNORM    0x100        /* Writing now will not block.  */
# define POLLWRBAND    0x200        /* Priority data may be written.  */
#endif

#ifdef __USE_GNU
/* These are extensions for Linux.  */
# define POLLMSG    0x400
# define POLLREMOVE    0x1000
# define POLLRDHUP     0x2000
#endif

/* Event types always implicitly polled for.  These bits need not be set in
   `events', but they will appear in `revents' to indicate the status of
   the file descriptor.  */
#define POLLERR        0x008        /* Error condition.  */
#define POLLHUP        0x010        /* Hung up.  */
#define POLLNVAL       0x020        /* Invalid polling request.  */
```

* 通常 poll 函数的调用过程如下

```cpp
std::vector<pollfd> pollfds(1);
pollfds[0].fd = listen_fd;
pollfds[0].events = POLLIN;

while (true) {
  constexpr int timeout = 1000;
  int ret = poll(pollfds.data(), pollfds.size(), timeout);
  if (ret == -1) {
    std::cout << "errno: " << strerror(errno) << '\n';
    break;
  }
  if (ret == 0) {
    std::cout << "timeout" << '\n';
    continue;
  }
  for (int i = 0; i < pollfds.size(); ++i) {
    if (pollfds[i].revents & POLLIN) {
      ...
    }
  }
}
```

## epoll

* epoll 在 Linux 2.5.44 中首次出现，使用前需要先创建一个 epfd，内核会分配一个对应的 eventpoll

```cpp
/* Creates an epoll instance.  Returns an fd for the new instance.
   The "size" parameter is a hint specifying the number of file
   descriptors to be associated with the new instance.  The fd
   returned by epoll_create() should be closed with close().  */
extern int epoll_create (int __size) __THROW;

/* Same as epoll_create but with an FLAGS parameter.  The unused SIZE
   parameter has been dropped.  */
extern int epoll_create1 (int __flags) __THROW;
```

* 用文件描述符生成一个 epoll_event 对象

```cpp
typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event
{
  uint32_t events;	/* Epoll events */
  epoll_data_t data;	/* User data variable */
} __EPOLL_PACKED;
```

* 随后将 epoll_event 添加到 epfd 中，eventpoll 以红黑树的数据结构存储 epitem，因此 epoll_ctl 的时间复杂度为 `O(log n)`。每个文件描述符只会有一次从用户态到内核的拷贝，不存在 select 和 poll 的重复拷贝的情况，[随着文件描述符数量增加，select 和 poll 的响应速度将线性下降，而 epoll 基本不受影响](http://www.xmailserver.org/linux-patches/nio-improve.html)。epoll_ctl 的实现中是有锁的，因此是线程安全的

```cpp
/* Valid opcodes ( "op" parameter ) to issue to epoll_ctl().  */
#define EPOLL_CTL_ADD 1	/* Add a file descriptor to the interface.  */
#define EPOLL_CTL_DEL 2	/* Remove a file descriptor from the interface.  */
#define EPOLL_CTL_MOD 3	/* Change file descriptor epoll_event structure.  */

/* Manipulate an epoll instance "epfd". Returns 0 in case of success,
   -1 in case of error ( the "errno" variable will contain the
   specific error code ) The "op" parameter is one of the EPOLL_CTL_*
   constants defined above. The "fd" parameter is the target of the
   operation. The "event" parameter describes which events the caller
   is interested in and any associated user data.  */
extern int epoll_ctl (int __epfd, int __op, int __fd,
		      struct epoll_event *__event) __THROW;
```

* 事件发生时，对应的文件描述符会被添加到一个链表中，epoll_wait 直接遍历链表即可获取所有就绪的文件描述符。epoll_wait 和 epoll_ctl 一样是线程安全的

```cpp
/* Wait for events on an epoll instance "epfd". Returns the number of
   triggered events returned in "events" buffer. Or -1 in case of
   error with the "errno" variable set to the specific error code. The
   "events" parameter is a buffer that will contain triggered
   events. The "maxevents" is the maximum number of events to be
   returned ( usually size of "events" ). The "timeout" parameter
   specifies the maximum wait time in milliseconds (-1 == infinite).

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int epoll_wait (int __epfd, struct epoll_event *__events,
		       int __maxevents, int __timeout);
```

* epoll 可以注册如下事件类型，其中 EPOLLET 表示边缘触发（Edge Triggered）模式，即 epoll 检查到事件触发后必须立即处理，下次不会再通知 epoll_wait，ET 减少了 epoll 事件重复触发的次数，结合 non-blocking 非常高效。如果不设置 ET，则默认使用电平触发（Level Triggered）模式，即此次未被处理的事件下次会继续通知 epoll_wait，直到被处理。LT 适合读数据，数据没读完下次会继续提醒，ET 适合写数据和接受数据

```cpp
enum EPOLL_EVENTS
  {
    EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
    EPOLLPRI = 0x002,
#define EPOLLPRI EPOLLPRI
    EPOLLOUT = 0x004,
#define EPOLLOUT EPOLLOUT
    EPOLLRDNORM = 0x040,
#define EPOLLRDNORM EPOLLRDNORM
    EPOLLRDBAND = 0x080,
#define EPOLLRDBAND EPOLLRDBAND
    EPOLLWRNORM = 0x100,
#define EPOLLWRNORM EPOLLWRNORM
    EPOLLWRBAND = 0x200,
#define EPOLLWRBAND EPOLLWRBAND
    EPOLLMSG = 0x400,
#define EPOLLMSG EPOLLMSG
    EPOLLERR = 0x008,
#define EPOLLERR EPOLLERR
    EPOLLHUP = 0x010,
#define EPOLLHUP EPOLLHUP
    EPOLLRDHUP = 0x2000,
#define EPOLLRDHUP EPOLLRDHUP
    EPOLLEXCLUSIVE = 1u << 28,
#define EPOLLEXCLUSIVE EPOLLEXCLUSIVE
    EPOLLWAKEUP = 1u << 29,
#define EPOLLWAKEUP EPOLLWAKEUP
    EPOLLONESHOT = 1u << 30,
#define EPOLLONESHOT EPOLLONESHOT
    EPOLLET = 1u << 31
#define EPOLLET EPOLLET
  };
```

* 通常 epoll 的调用过程如下

```cpp
int epfd = epoll_create1(0);
epoll_event ev;
ev.data.fd = fd;
ev.events = EPOLLIN | EPOLLET;
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK); // SetNonBlocking
std::vector<epoll_event> v(1000);
while (true) {
  int ret = epoll_wait(epfd, v.data(), v.size(), -1);
  if (ret == -1) {
    std::cout << "errno: " << strerror(errno) << '\n';
    break;
  }
  for (int i = 0; i < ret; ++i) {
    if (v[i].data.fd == listen_fd) {
      ... // do_accept
    } else if (v[i].events & EPOLLIN) {
      ... // do_read
    } else if (v[i].events & EPOLLOUT) {
      ... // do_write
    }
  }
}
```

## 基本 TCP 套接字 API

### [socket(2)](http://man.he.net/man2/socket)

* 为了执行网络 I/O，一个进程必须做的第一件事就是创建 socket 描述符。指定期望的协议类型，成功时返回一个小的非负整数值

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocal);
```

* domain：协议族，一般是 `AF_INET`，它决定了 socket 地址类型，如 `AF_INET` 决定要用 `32 位 IPv4 地址和 16 位端口号` 组合，常用的协议族有 `AF_INET(IPv4)、AF_INET6(IPv6)、AF_LOCAL（或称 AF_UNIX，Unix 域 socket）、AF_ROUTE`
* type：socket 类型，一般是 `SOCK_STREAM`，即 TCP，常用的有 `SOCK_STREAM（TCP）、SOCK_DGRAM（UDP）、SOCK_RAW（原始套接字）、SOCK_PACKET、SOCK_SEQPACKET`
* protocol：指定协议，为 0 时自动选择 type 类型对应的协议，常用的有 `IPPROTO_TCP、IPPTOTO_UDP、IPPROTO_SCTP、IPPROTO_TIPC`，注意 type 和 protocol 必须匹配，不能随意组合
* 示例

```cpp
int sock = socket(AF_INET, SOCK_STREAM, 0);
```

### [bind(2)](http://man.he.net/man2/bind)

* 给 socket 描述符绑定一个协议地址。如果调用 connect 或 listen 之前未用 bind 捆绑一个端口，内核就会为描述符分配一个临时端口，一般 TCP 客户端会采用这种做法，而 TCP 服务器应该在 listen 前 bind

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

* 示例

```cpp
sockaddr_in address; // #include <netinet/in.h>
address.sin_family = AF_INET;
address.sin_port = htons(12345);
address.sin_addr.s_addr = htonl(INADDR_ANY);
bzero(&(address.sin_zero), 8);
bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));
```

* sockaddr_in 定义如下

```cpp
struct sockaddr_in {
    short            sin_family;   // 2 bytes e.g. AF_INET, AF_INET6  
    unsigned short   sin_port;     // 2 bytes e.g. htons(3490)  
    struct in_addr   sin_addr;     // 4 bytes see struct in_addr, below  
    char             sin_zero[8];  // 8 bytes zero this if you want to  
};

struct in_addr {
    unsigned long s_addr;          // 4 bytes load with inet_pton()  
};

// sockaddr 和 sockaddr_in 大小都是 16 字节，只不过 sockaddr_in 把 14 个字节的 sa_data 拆开了
// sin_zero 用于填充字节，保证 sockaddr_in 和 sockaddr 一样大
struct sockaddr {
    unsigned short    sa_family;    // 2 bytes address family, AF_xxx  
    char              sa_data[14];  // 14 bytes of protocol address  
};

// sockaddr 是给系统用的，程序员应该用 sockaddr_in
// 通常用类型、IP 地址、端口填充 sockaddr_in 后，转换成 sockaddr 作为参数传递给调用函数
```

### [listen(2)](http://man.he.net/man2/listen)

* 仅由 TCP 服务器调用。用 socket 创建的套接字会被假设为主动套接字，即调用 connect 的客户端套接字。listen 将主动套接字转换成被动套接字，将套接字从 CLOSED 状态转换到 LISTEN 状态

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

* backlog：套接字（包括 SYS_RCVD 和 ESTABLISHED 队列）最大连接数，如果达到上限，客户端将收到 `ECONNREFUSED` 错误，一般设为 5
* listen 只适用于 `SOCK_STREAM` 和 `SOCK_SEQPACKET` 的 socket 类型，协议族为 `AF_INET` 时 `backlog` 最大可设为 128
* 示例

```cpp
listen(sock, 5);
```

### [accept(2)](http://man.he.net/man2/accept)

* 由 TCP 服务器调用，用于从 ESTABLISHED 队列头返回下一个已完成连接，如果队列为空则进程阻塞

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
* 第一个参数为服务端的监听描述符，accept 成功时返回一个自动生成的客户端描述符。监听描述符在服务器的生命期内一直存在，客户端描述符在服务器完成对给定客户的服务时关闭
* addr 和 addrlen 用来保存已连接的客户端的协议地址
* addrlen 调用前为 addr 所指的套接字地址结构的长度，返回时为由内核存放在该套接字地址结构内的确切字节数
* 示例

```cpp
sockaddr_in client;
socklen_t clientAddressLength = sizeof(client);
int clientFd = accept(sock, reinterpret_cast<sockaddr*>(&client), &clientAddressLength);
if (connectFd != -1)
{
    std::cout << "ip: " << inet_ntoa(client.sin_addr) << '\n';
    std::cout << "port: " << ntohs(client.sin_port) << '\n';
    close(clientFd);
}
```

### [connect(2)](http://man.he.net/man2/connect)

* 由 TCP 客户端调用，参数与 bind 相同，connect 前可以不 bind，内核会确定源 IP 地址并选择一个临时端口。如果是 TCP 套接字，调用 connect 将触发 TCP 的三路握手，且仅在连接成功或出错时才返回

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
```

* 示例

```cpp
sockaddr_in server;
bzero(&server, sizeof(server));
server.sin_family = AF_INET;
server.sin_port = htons(12345);
server.sin_addr.s_addr = inet_addr("192.168.211.129");
connect(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server));
```

### [close(2)](http://man.he.net/man2/close)

* 关闭套接字，终止 TCP 连接。并发服务器中父进程关闭已连接套接字只是导致相应描述符的引用计数值减 1，仅在该计数变为 0 时才关闭套接字

```cpp
#include <unistd.h>

int close(int sockfd);
```

* 示例

```cpp
close(sock);
```

### 字符处理函数

* 字节操控函数

```cpp
#include <strings.h>

void bzero(void *s, size_t n);
void bcopy(const void *src, void *dest, size_t n);
int bcmp(const void *s1, const void *s2, size_t n); // 相等返回 0，否则返回非 0
```

* 网络字节序通常为大端字节序，主机字节序通常为小端字节序。以下函数提供了主机字节序和网络字节序之间的转换

```cpp
#include <arpa/inet.h>

 // 主机字节序转网络字节序
unit16_t htons(unit16_t hostshort); // 一般用于端口号
uint32_t htonl(uint32_t hostlong); // 一般用于 IP 地址

 // 网络字节序转主机字节序
uint16_t ntohs(uint16_t netshort); // 一般用于端口号
uint32_t ntohl(uint32_t netlong); // 一般用于 IP 地址
```

* 点分十进制字符串（即 `xxx.xxx.xxx.xxx`）转网络字节序

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 将 cp 转换为 32 位网络字节序二进制值并存储在 inp 中。字符串有效返回 1，否则返回 0
int inet_aton(const char *cp, struct in_addr *inp);

// 将点分十进制字符串 cp 转为 IPv4 地址，字符串有效则返回 32 位二进制网络字节序的IPv4地址
// 否则为INADDR_NONE，通常是-1，即32位均为1，即255.255.255.255，这也意味着不能处理255.255.255.255
// 现在inet_addr已被废弃
in_addr_t inet_addr(const char *cp);

// 返回指向点分十进制字符串的指针，返回值所指向的字符串驻留在静态内存中，因此该函数是不可重入的
char *inet_ntoa(struct in_addr in);
```

* [inet_pton](http://man.he.net/man3/inet_pton) 和 [inet_ntop](http://man.he.net/man3/inet_ntop) 是随 IPv6 出现的新函数，对 IPv4 和 IPv6 都适用

```cpp
#include <arpa/inet.h>

// 成功返回1，输入不是有效的表达式返回0，出错返回-1
int inet_pton(int af, const char *src, void *dst);

// 成功返回指向结果的指针，出错返回NULL
const char* inet_ntop(int af, const void *src, char *dst, socklen_t size);

// 在<netinet/in.h>中有如下定义，可用作size
// 如果size太小,不足以容纳表达式结果,则返回一个空指针，并置errno为ENOSPC
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
```

* inet_pton 和 inet_ntop 示例

```cpp
const char* ip = "192.168.211.129";
inet_pton(AF_INET, ip, &foo.sin_addr); // foo.sin_add.s_addr = inet_addr(ip);

char s[INET_ADDRSTRLEN];
const char* p = inet_ntop(AF_INET, &foo.sin_addr, s, sizeof(s)); // p = inet_ntoa(foo.sin_addr);
```

### [fork(2)](man.he.net/man2/fork)

* [fork()](man.he.net/man2/fork) 创建一个新的子进程，在子进程中返回值为 0，在父进程中返回值为子进程的 PID，在子进程中可以通过 [getppid()](http://man.he.net/man2/getppid) 获取父进程的 PID，如果 fork 调用失败，则返回 -1

```cpp
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```

* 子进程和父进程继续执行 fork 调用之后的指令，但不确定先后顺序。子进程是父进程的副本，子进程不共享父进程的数据空间和堆栈，不过由于 fork 之后经常紧跟 exec，所以现在很多实现并不执行一个完全的副本，而是使用写时复制（Copy-On-Write，COW）技术，让父子空间共享区域，并将它们的访问权限改变为只读，如果任何一个父子进程要修改这些区域，内核会只为修改区域的那块内存制作一个副本，这个副本通常是虚拟内存中的一页

```cpp
int i = 0；
pid_t pid = fork();

if (pid == -1)
{
    std::cerr << "errno: " << errno << '\n';
}
else if (pid == 0)
{
    std::cout << "child id: " << getpid() << '\n';
    std::cout << "parent id:" << getppid() << '\n';
    ++i；
}
else
{
    std::cout << "child id: " << pid << '\n';
    std::cout << "parent id: " << getpid() << '\n';
}

std::cout << i; // 在子进程中为1，在父进程中为0
```

* fork 有两个典型用法
  * 一个进程创建一个自身的副本，这样每个副本都可以在另一个副本执行其他任务的同时处理各自的某个操作，这是网络服务器的典型用法
  * 一个进程想要执行另一个程序，先 fork 一个自身的副本，然后其中一个副本调用 exec 把自身替换成新的程序，这是 Shell 之类的程序的典型用法
