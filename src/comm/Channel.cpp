#include "Channel.h"
#include "ShmUtil.h"

#include <memory.h>
#include <strings.h>


/*
 * 初始化
 */
int	Channel_Init(Channel* pChannel, uint32_t iReqKey, uint32_t iRspKey)
{
	uint32_t dwReqSize = sizeof(RingQueue);
	int iRet = ShmUtil_GetShm2((void**)&pChannel->pReqQ, iReqKey, dwReqSize, 0666);
	if(iRet < 0)
	{
		iRet = ShmUtil_GetShm2((void**)&pChannel->pReqQ, iReqKey, dwReqSize, 0666 | IPC_CREAT);
		if(iRet < 0)
		{
			return -1;
		}

		memset(pChannel->pReqQ, 0, dwReqSize);
	}

	uint32_t dwRspSize = sizeof(RingQueue);
	iRet = ShmUtil_GetShm2((void**)&pChannel->pRspQ, iRspKey, dwRspSize, 0666);
	if(iRet < 0)
	{
		iRet = ShmUtil_GetShm2((void**)&pChannel->pRspQ, iRspKey, dwRspSize, 0666 | IPC_CREAT);
		if(iRet < 0)
		{
			return -2;
		}

		memset(pChannel->pRspQ, 0, dwRspSize);
	}

	return 0;
}

/*
 * 添加一个包入队列中，
 * 缓冲区满: 留一个包长距离
 * 
 * @param	iIsRequest		1	请求队列
 *							0	响应队列
 */
int Channel_AddPkg(Channel* pChannel, void *pBody, int iBodyLen,  int iIsRequest)
{
	ChannelPkgHead stChannelPkgHead;
	char* pBuffer = NULL;
	char* pCur = NULL;
	uint32_t* pdwHead = NULL;
	uint32_t* pdwTail = NULL;
	uint32_t* pdwTotalPkgs = NULL;
	uint32_t dwHead = 0;
	uint32_t dwTail = 0;
	uint32_t dwPkgLen = 0;

	//initialize
	if (iIsRequest)
	{
		pBuffer = pChannel->pReqQ->cBuf;
		pdwHead = &(pChannel->pReqQ->dwHead);
		pdwTail = &(pChannel->pReqQ->dwTail);
		pdwTotalPkgs = &(pChannel->pReqQ->dwTotalPkgs);
	}
	else
	{
		pBuffer = pChannel->pRspQ->cBuf;
		pdwHead = &(pChannel->pRspQ->dwHead);
		pdwTail = &(pChannel->pRspQ->dwTail);
		pdwTotalPkgs = &(pChannel->pRspQ->dwTotalPkgs);
	}

	dwHead = *pdwHead;
	dwTail = *pdwTail;

	// 错误检查
	if (dwHead >= CHANNEL_BUF_LEN || dwTail >= CHANNEL_BUF_LEN) 
	{
		//fatal error
		*pdwHead = *pdwTail = 0;
		return -1;
	}

	// 整个包长
	dwPkgLen = sizeof(stChannelPkgHead) + iBodyLen;

	if (dwTail >= dwHead)
	{		
		// tail>=dwHead, 判断留一个包长的距离
		if ((dwHead == 0) && (CHANNEL_BUF_LEN - dwTail == dwPkgLen)) 
		{
			return 1;
		}

		if(CHANNEL_BUF_LEN - dwTail < dwPkgLen)
		{
			if(dwHead <= dwPkgLen) 
			{
				return 1;
			}

			*(pBuffer + dwTail) = START_TAG;
			dwTail = 0;
		}
	}
	else
	{
		// tail < head, 已满，留一个包长的距离
		if (dwHead - dwTail <= dwPkgLen) 
		{
			return 1;
		}
	}

	// 定位当前写
	pCur = pBuffer + dwTail;

	// 添加包
	stChannelPkgHead.cFlag = START_TAG;
	stChannelPkgHead.wLength = iBodyLen;
	memcpy(pCur, &stChannelPkgHead, sizeof(stChannelPkgHead));
	memcpy(pCur+sizeof(stChannelPkgHead), pBody, iBodyLen);

	// 修改尾指针
	*pdwTail = (dwTail + dwPkgLen) % CHANNEL_BUF_LEN;
	(*pdwTotalPkgs) ++;
	return 0;
}

/*
 * 从队列中读取一个包
 * 缓冲区满: 留一个包长距离
 * 
 * @param	iIsRequest		1	入口服务器
 *							0	逻辑服务器
 */
int Channel_GetPkg(Channel* pChannel, void *pBody, int *piBodyLen, int iIsRequest)
{
	ChannelPkgHead *pstChannelPkgHead = NULL;
	char *pBuf = NULL;
	char *pCur = NULL;
	uint32_t* pdwTotalPkgs = NULL;
	uint32_t *pdwHead = NULL; 
	uint32_t *pdwTail = NULL;
	uint32_t dwHead = 0;
	uint32_t dwTail = 0;
	int iPkgLen = -1;
	int iBufLen = -1;

	if(pBody == NULL || piBodyLen == NULL)
	{
		return -1;
	}

	//initialize
	if (iIsRequest)
	{
		pBuf = pChannel->pReqQ->cBuf;
		pdwHead = &(pChannel->pReqQ->dwHead);
		pdwTail = &(pChannel->pReqQ->dwTail);
		pdwTotalPkgs = &(pChannel->pReqQ->dwTotalPkgs);
	}
	else
	{
		pBuf = pChannel->pRspQ->cBuf;
		pdwHead = &(pChannel->pRspQ->dwHead);
		pdwTail = &(pChannel->pRspQ->dwTail);
		pdwTotalPkgs = &(pChannel->pRspQ->dwTotalPkgs);
	}

	dwHead = *pdwHead;
	dwTail = *pdwTail;

	// 边界检查
	if (dwHead >= CHANNEL_BUF_LEN || dwTail >= CHANNEL_BUF_LEN) 
	{
		//fatal error
		*pdwHead = *pdwTail = 0;
		return -2;
	}

	// 已空，直接返回
	if (dwHead == dwTail) 
	{
		return 1;
	}

	// 到达边界
	if ( (dwHead > dwTail) && (*(pBuf + dwHead) == END_TAG) ) 
	{
		*pdwHead = dwHead = 0;
		
		if (dwHead == dwTail) 
		{
			return 1;
		}
	}

	// 获取buf的长度
	if (dwTail > dwHead)
	{
		iBufLen = dwTail - dwHead;
	}
	else 
	{
		// 先向前读.
		iBufLen = CHANNEL_BUF_LEN - dwHead;
	}

	// 双重检测, 出错
	if ((size_t)iBufLen < sizeof(ChannelPkgHead))
	{
		//fatal error
		*pdwHead = dwTail;
		return -2;
	}

	// 双重检测
	pstChannelPkgHead = (ChannelPkgHead *)(pBuf + dwHead);
	if (pstChannelPkgHead->cFlag != START_TAG)
	{
		//fatal error
		*pdwHead = dwTail;
		return -3;
	}

	iPkgLen = sizeof(ChannelPkgHead) + pstChannelPkgHead->wLength;

	// 尾部判断
	if ( (iBufLen < iPkgLen) || (*piBodyLen < pstChannelPkgHead->wLength) )
	{
		//fatal error
		*pdwHead = dwTail;
		return -4;
	}

	// 获取一个包
	pCur = pBuf + dwHead + sizeof(ChannelPkgHead);
	*piBodyLen = pstChannelPkgHead->wLength;
	memcpy(pBody, pCur, *piBodyLen);

	// 更改头部指针
	*pdwHead = (dwHead + iPkgLen) % CHANNEL_BUF_LEN;
	(*pdwTotalPkgs) --;

	return 0;
}

int Channel_GetPkgNum(Channel* pChannel, int iIsRequest)
{
	if(iIsRequest)
	{
		return pChannel->pReqQ->dwTotalPkgs;
	}
	else
	{
		return pChannel->pRspQ->dwTotalPkgs;
	}
}

