记录下windows下socket编程，直接贴代码。

服务端收到客户端数据后，显示客户端数据，并且回复`hello i am server`。

客户端发送数据，并且显示从服务端接收到的数据。

# 服务端：

```c++
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>  
#include <winsock2.h>  
#include <windows.h>  
#pragma comment(lib,"ws2_32.lib") 

struct TcpServer
{
	bool socketInit()
	{
		//初始化WSA  
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			return false;
		}

		//创建套接字  
		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverSocket == INVALID_SOCKET)
		{
			printf("socket error !");
			return false;
		}

		//绑定IP和端口  
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(8888);
		serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(serverSocket, (LPSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			printf("bind error !");
			return false;
		}
		printf("bind succ and server port : %d \n", ntohs(serverAddr.sin_port));

		//开始监听  
		if (listen(serverSocket, 5) == SOCKET_ERROR)
		{
			printf("listen error !");
			return false;
		}
		return true;
	}

	bool socketListen(SOCKET& clientSocket)
	{
		sockaddr_in clientAddr;
		int nAddrlen = sizeof(clientAddr);
		printf("wait a new connect...\n");
		clientSocket = accept(serverSocket, (SOCKADDR *)&clientAddr, &nAddrlen);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("accept error !");
			false;
		}
		printf("new connect： client addr :%s , client port : %d\r\n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
		return true;
	}

	void socketFree()
	{
		closesocket(serverSocket);
		WSACleanup();
	}
private:
	SOCKET serverSocket;
};

DWORD WINAPI clientThread(void *args)
{
	SOCKET clientSocket = *((SOCKET*)args);
	char revData[255];
	while (true)
	{
		int recvLen = recv(clientSocket, revData, 255, 0);
		if (recvLen > 0)
		{
			revData[recvLen] = 0x00;
			printf("%s\n",revData);
			const char * sendData = "hello i am server\n";
			send(clientSocket, sendData, strlen(sendData), 0);
		}
		else
		{
			closesocket(clientSocket);
			printf("client disconnected!\n");
			return 0;
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	TcpServer server;
	server.socketInit();
	while (true)
	{
		SOCKET clientSocket;
		if (server.socketListen(clientSocket))
		{
			HANDLE hThread;
			DWORD  threadId;
			hThread = CreateThread(NULL, 0, clientThread, (void *)&clientSocket, 0, &threadId);
		}
	}
	server.socketFree();
	return 0;
}
```



# 客户端：

```c++
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h> 
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

struct TcpClient
{
	bool socketInit()
	{
		WSADATA data;
		if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		{
			return false;
		}

		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("invalid socket!");
			return false;
		}
		return true;
	}

	bool socketConnect()
	{
		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		if (connect(clientSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{ 
			printf("connect error !");
			closesocket(clientSocket);
			return false;
		}
		return true;
	}

	void socketSend()
	{
		string data;
		cin >> data;
		const char * sendData = data.c_str();  
		send(clientSocket, sendData, strlen(sendData), 0);
	
		char recData[255];
		int ret = recv(clientSocket, recData, 255, 0);
		if (ret > 0)
		{
			recData[ret] = 0x00;
			printf("%s\n", recData);
		}
	}

	void socketFree()
	{
		closesocket(clientSocket);
		WSACleanup();
	}
private:
	SOCKET clientSocket;
};

int main()
{
	TcpClient client;

	if (!client.socketInit())
		return 0;

	if (!client.socketConnect())
		return 0;

	while (true)
	{
		client.socketSend();
	}
	client.socketFree();
	return 0;

}
```



# HTTP:

使用TCP模拟HTTP发送 HTTP响应给postman。



```c++
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>  
#include <winsock2.h>  
#include <windows.h>  
#include <string>
#pragma comment(lib,"ws2_32.lib") 
using namespace std;

struct HttpServer
{
	bool socketInit()
	{
		//初始化WSA  
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			return false;
		}

		//创建套接字  
		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverSocket == INVALID_SOCKET)
		{
			printf("socket error !");
			return false;
		}

		unsigned long arg = 0;
		int nRet = ioctlsocket(serverSocket, FIONBIO, &arg);

		//绑定IP和端口  
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(8888);
		serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(serverSocket, (LPSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			printf("bind error !");
			return false;
		}
		printf("bind succ and server port : %d \n", ntohs(serverAddr.sin_port));

		//开始监听  
		if (listen(serverSocket, 5) == SOCKET_ERROR)
		{
			printf("listen error !");
			return false;
		}
		return true;
	}

	bool socketListen(SOCKET& clientSocket)
	{
		sockaddr_in clientAddr;
		int nAddrlen = sizeof(clientAddr);
		printf("\nwait a new connect...\n");
		clientSocket = accept(serverSocket, (SOCKADDR *)&clientAddr, &nAddrlen);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("accept error !");
			false;
		}
		printf("new connect： client addr :%s , client port : %d\r\n", inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));
		return true;
	}

	void socketFree()
	{
		closesocket(serverSocket);
		WSACleanup();
	}
private:
	SOCKET serverSocket;
};

#define RESP_HTTP_POST_FORMAT           "HTTP/1.1 200 OK\r\n"  \
                                        "Content-Type:application/json;charset=UTF-8\r\n"   \
                                        "Content-Length:%d\r\n"    \
                                        "Date: 111111111 GMT\r\n\r\n"    \
                                        "%s\r\n"    \

DWORD WINAPI clientThread(void *args)
{
	printf("create client thread\n");
	SOCKET clientSocket = *((SOCKET*)args);
	char revData[1024];
	int nLen = 0;
	char sendData[1024] = { 0 };
	std::string strBody = "{\"1\":1}";
	sprintf(sendData, RESP_HTTP_POST_FORMAT, strBody.length(), strBody.c_str());
	while (true)
	{
		int recvLen = recv(clientSocket, revData, 1024, 0);
		if (recvLen > 0)
		{
			printf("recv data from clientSocket[%d]\n", clientSocket);
			nLen = send(clientSocket, sendData, strlen(sendData), 0);
		}
		else
		{
			closesocket(clientSocket);
			printf("client disconnected! error = %d\n", GetLastError());
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	HttpServer server;
	server.socketInit();
	while (true)
	{
		SOCKET clientSocket;
		if (server.socketListen(clientSocket))
		{
			HANDLE hThread;
			DWORD  threadId;
			hThread = CreateThread(NULL, 0, clientThread, (void *)&clientSocket, 0, &threadId);
		}
	}
	server.socketFree();
	return 0;
}
```



# 参考文献：

1. [linux IO 模型](https://github.com/downdemo/IO-Multiplexing)
2. [TCP服务器如何判断Client断开](https://www.cnblogs.com/youxin/p/4056041.html)
3. [服务器中判断客户端socket断开连接的方法](https://www.cnblogs.com/jacklikedogs/p/3976208.html)
4. [由socket的accept说开去](https://blog.51cto.com/ticktick/779866)