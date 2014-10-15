#include "Channel.h"
#include "ShmUtil.h"

#include <memory.h>
#include <strings.h>


/*
 * ��ʼ��
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
 * ���һ����������У�
 * ��������: ��һ����������
 * 
 * @param	iIsRequest		1	�������
 *							0	��Ӧ����
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

	// ������
	if (dwHead >= CHANNEL_BUF_LEN || dwTail >= CHANNEL_BUF_LEN) 
	{
		//fatal error
		*pdwHead = *pdwTail = 0;
		return -1;
	}

	// ��������
	dwPkgLen = sizeof(stChannelPkgHead) + iBodyLen;

	if (dwTail >= dwHead)
	{		
		// tail>=dwHead, �ж���һ�������ľ���
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
		// tail < head, ��������һ�������ľ���
		if (dwHead - dwTail <= dwPkgLen) 
		{
			return 1;
		}
	}

	// ��λ��ǰд
	pCur = pBuffer + dwTail;

	// ��Ӱ�
	stChannelPkgHead.cFlag = START_TAG;
	stChannelPkgHead.wLength = iBodyLen;
	memcpy(pCur, &stChannelPkgHead, sizeof(stChannelPkgHead));
	memcpy(pCur+sizeof(stChannelPkgHead), pBody, iBodyLen);

	// �޸�βָ��
	*pdwTail = (dwTail + dwPkgLen) % CHANNEL_BUF_LEN;
	(*pdwTotalPkgs) ++;
	return 0;
}

/*
 * �Ӷ����ж�ȡһ����
 * ��������: ��һ����������
 * 
 * @param	iIsRequest		1	��ڷ�����
 *							0	�߼�������
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

	// �߽���
	if (dwHead >= CHANNEL_BUF_LEN || dwTail >= CHANNEL_BUF_LEN) 
	{
		//fatal error
		*pdwHead = *pdwTail = 0;
		return -2;
	}

	// �ѿգ�ֱ�ӷ���
	if (dwHead == dwTail) 
	{
		return 1;
	}

	// ����߽�
	if ( (dwHead > dwTail) && (*(pBuf + dwHead) == END_TAG) ) 
	{
		*pdwHead = dwHead = 0;
		
		if (dwHead == dwTail) 
		{
			return 1;
		}
	}

	// ��ȡbuf�ĳ���
	if (dwTail > dwHead)
	{
		iBufLen = dwTail - dwHead;
	}
	else 
	{
		// ����ǰ��.
		iBufLen = CHANNEL_BUF_LEN - dwHead;
	}

	// ˫�ؼ��, ����
	if ((size_t)iBufLen < sizeof(ChannelPkgHead))
	{
		//fatal error
		*pdwHead = dwTail;
		return -2;
	}

	// ˫�ؼ��
	pstChannelPkgHead = (ChannelPkgHead *)(pBuf + dwHead);
	if (pstChannelPkgHead->cFlag != START_TAG)
	{
		//fatal error
		*pdwHead = dwTail;
		return -3;
	}

	iPkgLen = sizeof(ChannelPkgHead) + pstChannelPkgHead->wLength;

	// β���ж�
	if ( (iBufLen < iPkgLen) || (*piBodyLen < pstChannelPkgHead->wLength) )
	{
		//fatal error
		*pdwHead = dwTail;
		return -4;
	}

	// ��ȡһ����
	pCur = pBuf + dwHead + sizeof(ChannelPkgHead);
	*piBodyLen = pstChannelPkgHead->wLength;
	memcpy(pBody, pCur, *piBodyLen);

	// ����ͷ��ָ��
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

