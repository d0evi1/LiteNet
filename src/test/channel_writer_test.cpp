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
		printf("Channel::Init() fail: iRet = %d\n", iRet);
		return -1;
	}

	return 0;
}

// 变长包体，size = 104
typedef struct 
{
	int		iLen;
	char	szBuf[50];
} ReqPkg;

static void PackReqPkg(ReqPkg* pstPkg, char* acPkg, int* piLen)
{
	memcpy(acPkg, &(pstPkg->iLen), sizeof(int));
	memcpy(acPkg+sizeof(int), &(pstPkg->szBuf), sizeof(char) * pstPkg->iLen);
	*piLen = sizeof(int) + sizeof(char) * pstPkg->iLen;
}

static void UnpackReqPkg(char* acPkg, int iLen, ReqPkg* pstPkg)
{
	memcpy(pstPkg, acPkg, sizeof(int));
	memcpy(pstPkg+sizeof(int), acPkg+sizeof(int), pstPkg->iLen);
}


int main()
{
	if( InitChannel() < 0 )
	{
		return -1;
	}

	ReqPkg stPkg;
	bzero(&stPkg, sizeof(ReqPkg));

	char* szHello = "hello world";
	stPkg.iLen = strlen(szHello);
	memcpy(stPkg.szBuf, szHello, sizeof(stPkg.szBuf));

	char cBuf[65535];
	int iLen = sizeof(cBuf);
	PackReqPkg(&stPkg, cBuf, &iLen);

	for(int i=0; i<=100; i++)
	{
		int iRet = g_Channel.AddPkg(cBuf, iLen, 1);
		if(iRet != 0)
		{
			printf("AddPkg fail: i=%d, iRet=%d, head=%u, tail=%u\n", 
				i, iRet, g_Channel.m_pReqQ->dwHead, g_Channel.m_pReqQ->dwTail);
			return -1;
		}
	}

	return 0;
}