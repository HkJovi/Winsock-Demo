// SocketServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment (lib,"ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建套接字
	SOCKET sLisent = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//对sockaddr_in结构体进行填充地址端口等等。
	struct sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("10.128.254.14");
	ServerAddr.sin_port = htons(1234); 

	bind(sLisent, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));

	listen(sLisent, SOMAXCONN);

	sockaddr_in ClientAddr;
	int nSize = sizeof(ClientAddr);

	SOCKET sClient = accept(sLisent, (SOCKADDR *)&ClientAddr, &nSize);

	printf("Client IP = %s:%d \n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));


	char szMsg[MAXBYTE] = { 0 };
	lstrcpy(szMsg, "hello Client !\n");
	send(sClient, szMsg, strlen(szMsg) + sizeof(char), 0);

	recv(sClient, szMsg, MAXBYTE, 0);
	printf("Client Msg: %s", szMsg);

	WSACleanup();
	system("pause");
	return 0;
}

