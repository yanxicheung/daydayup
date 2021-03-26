#include <sys/types.h>         
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

int createSocket()
{
	int listenSocket;
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	//设置服务器协议族;地址和端口号;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6001);
	server_addr.sin_addr.s_addr = inet_addr("10.67.76.9");

	if (bind(listenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		close(listenSocket);
		return -1;
	}

	if (listen(listenSocket, 512) < 0){
		close(listenSocket);
		return -1;
	}
	return listenSocket;
}


int main(int argc, char *argv[])
{
	int listenSocket = createSocket();
	struct sockaddr_in client_addr;
	socklen_t  sock_len = sizeof(client_addr);
	for (;;)
	{
		int clientSocket = accept(listenSocket, (struct sockaddr*)&client_addr, &sock_len);
		printf("Client:%s,Port:%d connected,clientSocket = %d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), clientSocket);
		nRet = recv(clientSocket, ite->second->pTmpBuff, RECV_BUF_LEN, 0);
	}
	return 0;
}

