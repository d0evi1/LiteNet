#include <stdio.h>

#include "comm/Channel.h"

#define REQ_QUEUE_KEY		0x20121101
#define RSP_QUEUE_KEY		0x20121102
Channel g_Channel;

int InitChannel()
{
	int iRet = g_Channel.Init(REQ_QUEUE_KEY, RSP_QUEUE_KEY);
	if(iRet < 0)
	{
		printf("Channel::Init() fail.\n");
		return -1;
	}

	return 0;
}

struct Pkg
{
	int		iLen;
	char	szBuf[100];
};


int main()
{
	if( InitChannel() < 0 )
	{
		return -1;
	}

	Pkg stPkg;
	bzero(&stPkg, sizeof(Pkg));
	int iLen = sizeof(stPkg);

	for(int i=0; i<=100; i++)
	{
		int iRet = g_Channel.GetPkg(&stPkg, &iLen, 1);
		if(iRet != 0)
		{
			printf("GetPkg fail: i=%d, iRet=%d, head=%u, tail=%u\n", 
				i, iRet, g_Channel.m_pReqQ->dwHead, g_Channel.m_pReqQ->dwTail);
			return -2;
		}

		printf("iLen = %d\n", stPkg.iLen);
		printf("szBuf = %s\n", stPkg.szBuf);
	}

	return 0;
}