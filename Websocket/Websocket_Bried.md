# Quick Start：

## 简介：

[WebSocket](http://websocket.org/) 是一种网络通信协议，是html5提出的一个协议规范，很多高级功能都需要它。

WebSocket基于tcp，先通过HTTP/HTTPS协议发起一条特殊的http请求进行握手后创建一个用于交换数据的TCP连接。**Websocket默认支持长连接。使用ES7新标准的async/await特性，可以实现WebSocket的短连接**。



初次接触 WebSocket 的人，都会问同样的问题：我们已经有了 HTTP 协议，为什么还需要另一个协议？它能带来什么好处？

答案很简单，因为 **HTTP 协议有一个缺陷**：**通信只能由客户端发起**。

举例来说，我们想了解今天的天气，只能是客户端向服务器发出请求，服务器返回查询结果。HTTP 协议做不到服务器主动向客户端推送信息。

 WebSocket的最大特点就是，服务器可以主动向客户端推送信息，客户端也可以主动向服务器发送信息，是真正的双向平等对话，属于[服务器推送技术](https://en.wikipedia.org/wiki/Push_technology)的一种。

![image-20210309134650418](res/websocket.png)

其他特点包括：

（1）建立在 TCP 协议之上，服务器端的实现比较容易。

（2）与 HTTP 协议有着良好的兼容性。默认端口也是80和443，并且握手阶段采用 HTTP 协议，因此握手时不容易屏蔽，能通过各种 HTTP 代理服务器。

（3）数据格式比较轻量，性能开销小，通信高效。

（4）可以发送文本，也可以发送二进制数据。

（5）没有同源限制，客户端可以与任意服务器通信。

（6）协议标识符是`ws`（如果加密，则为`wss`），服务器网址就是 URL。



## 协议格式：

![image-20210309134538053](res/wsProtocl.png)

## 连接：

websocket连接时使用HTTP协议，但是在建立连接之后，真正的数据传输阶段是不需要HTTP协议参与。

连接建立过程:

1. 客户端发送GET 请求，这里使用HTTP协议

```http
GET / HTTP/1.1
Pragma: no-cache
Cache-Control: no-cache
Host: 127.0.0.1:28709
Origin: http://127.0.0.1:28709
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: O4jsTzgFMwr1ZFGc+up8sQ==
Sec-WebSocket-Protocol: daniel
Sec-WebSocket-Version: 13
```

2. 服务器给客户端 switching protocol

```HTTP
HTTP/1.1 101 Switching Protocols
Upgrade: WebSocket
Connection: Upgrade
Sec-WebSocket-Accept: z78220qKfCtVo4H2C+/tIvzFjZE=
Sec-WebSocket-Protocol: daniel
```

`Sec-WebSocket-Protocol`仅用于WebSocket打开阶段握手，它从客户端发送到服务器端。这个字段不是强制的，可以带也可以不带。

实际抓包截图：

![image-20210309135007280](res/wsConnect.png)


# 参考文献：

1. [(扫盲)WebSocket 教程](https://www.cnblogs.com/ximenxiazi/p/9233626.html)
2. [C++ 使用 websocket总结](https://zhuanlan.zhihu.com/p/250276105)
3. [WebSocket 的诞生](https://www.cnblogs.com/zhangmingda/p/12678630.html)
4. [WebSocket 连接建立过程](https://www.jianshu.com/p/bae7ea3e9adb)
5. [C++ libwebsockets搭建WebSocket服务端及Http客户端、服务端](https://blog.csdn.net/fantasysolo/article/details/88908948)

