#include <sys/types.h>         
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

sockaddr_in sockAddress(const char*ip, int port) {
  sockaddr_in res;
  res.sin_family = AF_INET;
  res.sin_port = htons(port);
  inet_pton(AF_INET, ip,
            &res.sin_addr);
  bzero(&(res.sin_zero), 8);
  return res;
}

int createSocket()
{
	int listenSocket;
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	
    int reuse = 1;
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
    	return -1;
    }
  
	//设置协议族;地址和端口号;
    sockaddr_in server_addr = sockAddress("192.168.101.6", 6002);
	if (bind(listenSocket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		close(listenSocket);
		return -1;
	}

	if (listen(listenSocket, 512) < 0){
		close(listenSocket);
		return -1;
	}
	return listenSocket;
}

/* 用于演示阻塞和非阻塞IO */
int main(int argc, char *argv[])
{
	int listenSocket = createSocket();
	
	sockaddr_in client_addr;
	socklen_t  sock_len = sizeof(client_addr);
	
	char buffer[64] = {0};
	for (;;)
	{
		int clientSocket = accept(listenSocket, (sockaddr*)&client_addr, &sock_len);
		printf("Client:%s, Port:%d connected, clientSocket = %d\n",\
		inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), clientSocket);
		
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

