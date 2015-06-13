// MyPing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdafx.h"
#include <Winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

void InitSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return;
	}
}

//icmp包结构
typedef struct _ICMPPACK
{
	unsigned char icmp_type;  //消息类型
	unsigned char icmp_code;  //清息代码
	unsigned short icmp_checksum; //16位效验

	unsigned short icmp_id;//用来唯一标识些请求的ID号
	unsigned short icmp_sequence; //序列号
	unsigned long icmp_timestamp; //时间戳

}ICMPPACK, *PICMPPACK;

BOOL SetTimeOut(SOCKET s, int nTime, BOOL bRecv)
{
	int nRet = setsockopt(s, SOL_SOCKET, bRecv ? SO_RCVTIMEO : SO_SNDTIMEO, (char *)&nTime, sizeof(nTime));
	if (SOCKET_ERROR == nRet)
	{
		return false;
	}
	return true;
}

int GetTTL(SOCKET s)
{
	int TTLCounts;
	int nLen = sizeof(TTLCounts);
	int nRet = getsockopt(s, IPPROTO_IP, IP_TTL, (char *)&TTLCounts, &nLen);
	return TTLCounts;
}

unsigned short checksum(unsigned short *buff, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buff++;
		size -= sizeof(unsigned short);
	}
	if (size)
	{
		cksum += *(unsigned char*)buff;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += cksum >> 16;
	return (unsigned short)(~cksum);
}

int _tmain(int argc, _TCHAR* argv[])
{
	InitSocket();
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (INVALID_SOCKET == sRaw)
	{
		printf("failed to create raw socket\n");
		return 0;
	}
	if (!SetTimeOut(sRaw, 1000, true))
	{
		printf("failed to set recv time out\n");
		closesocket(sRaw);
		return 0;
	}
	//设置目标地址
	sockaddr_in desAddr;
	desAddr.sin_addr.S_un.S_addr = inet_addr("10.0.132.165");
	desAddr.sin_port = htons(0);
	desAddr.sin_family = AF_INET;

	char buff[sizeof(ICMPPACK) + 32];  //创建ICMP包
	PICMPPACK pICMP = (PICMPPACK)buff;

	pICMP->icmp_type = 8; //请求回显
	pICMP->icmp_code = 0;
	pICMP->icmp_checksum = 0;
	pICMP->icmp_id = (unsigned short)GetCurrentProcessId();
	pICMP->icmp_sequence = 0;
	pICMP->icmp_timestamp = 0;

	memset(&buff[sizeof(ICMPPACK)], 'E', 32);  //填充用户数据

	unsigned short nSeq = 0;
	char recvBuf[1024];
	sockaddr_in from;
	int nLen = sizeof(sockaddr_in);
	int count = 0;
	int nRet;
	while (true)
	{
		if (count++ == 4)
		{
			break;
		}
		pICMP->icmp_checksum = 0;
		pICMP->icmp_sequence = nSeq++;
		pICMP->icmp_timestamp = ::GetTickCount();
		pICMP->icmp_checksum = ::checksum((unsigned short *)buff, sizeof(ICMPPACK) + 32);
		nRet = sendto(sRaw, buff, sizeof(ICMPPACK) + 32, 0, (sockaddr *)&desAddr, sizeof(desAddr));
		if (nRet == SOCKET_ERROR)
		{
			printf("sendto() failed \n");
			closesocket(sRaw);
			return 0;
		}
		nRet = recvfrom(sRaw, recvBuf, 1024, 0, (sockaddr *)&from, &nLen);
		if (nRet == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				printf("time out \n");
				continue;
			}
			else
			{
				printf("recvfrom() failed \n");
				closesocket(sRaw);
				return 0;
			}
		}
		int nTick = GetTickCount();

		if (nRet < 20 + sizeof(ICMPPACK))
		{
			printf("too few bytes from %s \n", ::inet_ntoa(from.sin_addr));
			continue;
		}
		ICMPPACK *pTemp = (ICMPPACK *)(recvBuf + 20);
		if (pTemp->icmp_type != 0)
		{
			printf(" nonecho type recvd \n");
			closesocket(sRaw);
			return 0;
		}
		if (pTemp->icmp_id != (unsigned short)GetCurrentProcessId())
		{
			printf("some one else packet");
			closesocket(sRaw);
			return 0;
		}
		printf("%d Replay from %s: bytes=%d time<%dms TTL=%d\n", pTemp->icmp_sequence, inet_ntoa(from.sin_addr), nRet, nTick - pTemp->icmp_timestamp, GetTTL(sRaw));
 		::Sleep(2000);
	}
	system("pause");
	return 0;
}