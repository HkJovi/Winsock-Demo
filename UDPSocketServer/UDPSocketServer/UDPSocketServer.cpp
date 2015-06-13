// UDPSocketServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sSever = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("10.128.254.14");
	ServerAddr.sin_port = htons(1234);

	bind(sSever, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));

	char szMsg[MAXBYTE] = { 0 };
	sockaddr_in ClientAddr;
	int nSize = sizeof(ClientAddr);
	recvfrom(sSever, szMsg, MAXBYTE, 0, (SOCKADDR *)&ClientAddr, &nSize);

	printf("Client Msg:%s\n", szMsg);
	printf("Client IP = %s:%d", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));

	lstrcpy(szMsg, "Hello Client \n");
	nSize = sizeof(ClientAddr);
	sendto(sSever, szMsg, strlen(szMsg) + sizeof(char), 0, (SOCKADDR *)&ClientAddr, nSize);

	WSACleanup();
	system("pause");
	return 0;
}

