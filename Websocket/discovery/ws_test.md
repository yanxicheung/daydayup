# libwebsocket：

libwebsockets是一款轻量级用来开发服务器和客户端的C库。按照官方（https://libwebsockets.org/）给出的介绍来看，它不仅**支持ws，wss**还同时**支持http与https**，可以轻轻松松结合openssl等库来实现ssl加密。

源代码地址：https://github.com/warmcat/libwebsockets

使用websocket测试工具：http://wstool.jackxiang.com



# 编译

解压后在根目录下创建build目录用于编译，进入build目录，编译指令：

```
cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/../output -DCMAKE_C_FLAGS=-fPIC
make
make install
```

参考：[linux下libwebsockets源码编译以及测试](https://blog.csdn.net/lang523493505/article/details/121116458)



# reason：

对于ws服务器正常流程下重要的回调枚举值：

| 状态码                        | 值   | 含义                                                         |
| ----------------------------- | ---- | ------------------------------------------------------------ |
| LWS_CALLBACK_ESTABLISHED      | 0    | ws服务端和客户端建立起连接。                                 |
| LWS_CALLBACK_RECEIVE          | 6    | 收到一帧完整数据注，需要注意的是：指针的回收、释放始终由LWS框架管理，只要出了回调函数，该空间就会被LWS框架回收。因此，开发者若想将接收的数据进行转发，则必须对该数据进行拷贝。 |
| LWS_CALLBACK_SERVER_WRITEABLE | 11   | 此ws连接为可写状态注：表示wsi对应的ws连接当前处于可写状态，即：可发送数据至客户端。 |
| LWS_CALLBACK_CLOSED           | 4    | ws连接已经断开，不能在此释放内存空间，否则存在内存泄漏的风险！！！因为连接断开时，并不总是会回调LWS_CALLBACK_CLOSED的处理！ |



对于http服务器正常流程下重要的回调枚举值：

| 状态码                      | 值   | 含义               |
| --------------------------- | ---- | ------------------ |
| LWS_CALLBACK_HTTP           | 12   | 收到http消息头     |
| LWS_CALLBACK_HTTP_BODY      | 13   | 收到http消息体     |
| LWS_CALLBACK_HTTP_WRITEABLE | 16   | 链路已准备好，可写 |
| LWS_CALLBACK_CLOSED_HTTP    | 5    | 收到断开连接       |



# example：

## simple

最简单的服务端和客户端的收发数据的示例。

```c
struct lws_protocols protocols[] = {
    {
        //协议名称，协议回调，接收缓冲区大小
        "daniel", callback, sizeof( struct session_data ), MAX_PAYLOAD_SIZE,
    },
    {
        NULL, NULL,   0 // 最后一个元素固定为此格式
    }
};
```

`daniel`为子协议名称，即`ws`握手时，携带的`Sec-WebSocket-Protocol`字段。



`server.c`对应服务端，里面的端口可以按照需要修改，编译出的可执行文件为`server`。

`client.c`对应客户端，里面连接服务端的地址、端口可以按需修改，编译出的可执行文件为`client`。



`ws_cap.pcap`为`server`和`client`之间通信的抓包文件。

`ws_server_and_web.pcap`为`server`和[web ws测试工具](http://wstool.jackxiang.com)之间通信的抓包文件，测试工具的服务地址示例为：

`ws://10.67.76.9:28709`





# 参考文献

1. [libwebsockets库的简易教程](https://blog.csdn.net/shihoongbo/article/details/113929963)
2. [libwebsockets搭建WebSocket服务端及Http客户端、服务端](https://blog.csdn.net/fantasysolo/article/details/88908948)