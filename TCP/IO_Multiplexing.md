```c++
int main(int argc, char *argv[])
{
	int listenSocket = createSocket();
	
	sockaddr_in client_addr;
	socklen_t  sock_len = sizeof(client_addr);
	
	char buffer[64] = {0};
	for (;;)
	{
		int clientSocket = accept(listenSocket, (sockaddr*)&client_addr, &sock_len);
	    fcntl(clientSocket, F_SETFL, O_NONBLOCK);  // 设置为非阻塞IO
	    
	    int nRet = recv(clientSocket, buffer, sizeof(buffer), 0);
	    if(nRet > 0)
	    {
	    	printf("recv %s , len = %d\n",buffer, nRet);
	        close(clientSocket);
	    }
	    else
	    {
	    //  Resource temporarily unavailable
	    	printf("nRet = %d, [err] %s\n",nRet, strerror(errno));
	    }
	}
	return 0;
}
```









# 参考：

1. [IO多路复用](https://mp.weixin.qq.com/s/3gC-nUnFGv-eoSBsEdSZuA)

