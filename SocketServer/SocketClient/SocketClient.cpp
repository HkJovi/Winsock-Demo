// SocketClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32")


int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("10.128.254.14");
	ServerAddr.sin_port = htons(1234);

	connect(sServer, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));

	char szMsg[MAXBYTE] = { 0 };
	recv(sServer, szMsg, MAXBYTE, 0);
	printf("Server Msg: %s\n", szMsg);

	lstrcpy(szMsg, "hello Server!");
	send(sServer, szMsg, strlen(szMsg) + sizeof(char), 0);

	WSACleanup();
	system("pause");
	return 0;
}

