// UDPSocketClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sSever = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("10.128.254.14");
	ServerAddr.sin_port = htons(1234);

	char szMsg[MAXBYTE] = { 0 };
	lstrcpy(szMsg, "Hello Server \n");
	int nSize = sizeof(ServerAddr);
	sendto(sSever, szMsg, strlen(szMsg) + sizeof(char), 0, (SOCKADDR *)&ServerAddr, nSize);

	nSize = sizeof(ServerAddr);
	recvfrom(sSever, szMsg, MAXBYTE, 0, (SOCKADDR *)&ServerAddr, &nSize);

	printf("Server Msg:%s\n", szMsg);

	WSACleanup();
	system("pause");
	return 0;
}

