#include "SocketOutputStream.h"
#include "SocketAPI.h"

#define ERR_LENGTH		0xffffffff


/*
 * ��ʼ��, �������˴��룬 ֻ�����һ�γ�ʼ��
 */
int SocketOutputStream_Init(SocketOutputStream* pStream, int iSocket)
{
	if(pStream == NULL)
	{
		return -1;
	}
	
	memset(pStream, 0, sizeof(SocketOutputStream)); 
	pStream->iSocket = iSocket;
	
	return 0;
}

/*
 * ���ض��г���.
 */
uint32_t SocketOutputStream_Length(SocketOutputStream* pStream)
{
	if(pStream == NULL)
	{
		return ERR_LENGTH;
	}
	
	uint32_t dwHead = pStream->dwHead;
	uint32_t dwTail = pStream->dwTail;

	if(dwHead < dwTail)
	{
		return (dwTail - dwHead);
	}
	else if(dwHead > dwTail) 
	{
		return (MAX_OUTPUTSTREAM_SIZE - pStream->dwHead + pStream->dwTail);
	}

	return ERR_LENGTH;
}

/*
 * ���ض�������
 */
uint32_t SocketOutputStream_Capacity(SocketOutputStream* pStream)
{
	return MAX_OUTPUTSTREAM_SIZE; 
}

/*
 * �������Ƿ�Ϊ��
 */
int	SocketOutputStream_IsEmpty(SocketOutputStream* pStream)
{ 
	return (pStream->dwHead == pStream->dwTail ? 1 : 0); 
}

/*
 * �򻺳���д����
 */
int SocketOutputStream_Write(SocketOutputStream* pStream, const char* cBuf, uint32_t* pdwLen) 
{
	if(pStream == NULL || cBuf == NULL || pdwLen == NULL)
	{
		return -1;
	}

	uint32_t dwLen = *pdwLen;
	uint32_t dwHead = pStream->dwHead;
	uint32_t dwTail = pStream->dwTail;

	//					//
	//     T  H			//    H   T			LEN=10
	// 0123456789		// 0123456789
	// abcd...efg		// ...abcd...
	//					//

	// step 1: ��ѯʣ��ռ�, �Ƿ����·���
	uint32_t nFree = 0;
	if(dwHead <= dwTail)
	{
		nFree = MAX_OUTPUTSTREAM_SIZE - dwTail + dwHead - 1;
	}
	else
	{
		nFree = dwHead - dwTail - 1;
	}

	if(dwLen >= nFree)
	{
		return -2;
	}

	// 
	if(dwHead <= dwTail) 
	{	
		if(dwHead == 0) 
		{
			nFree = MAX_OUTPUTSTREAM_SIZE - dwTail - 1;
			memcpy(&pStream->cBuf[dwTail], cBuf, dwLen);
		} 
		else 
		{
			nFree = MAX_OUTPUTSTREAM_SIZE - dwTail;
			if(dwLen <= nFree)
			{
				memcpy(&pStream->cBuf[dwTail], cBuf, dwLen);
			}
			else 
			{
				memcpy(&pStream->cBuf[dwTail], cBuf, nFree);
				memcpy(pStream->cBuf, &cBuf[nFree], dwLen - nFree);
			}
		}
	} 
	else 
	{	
		memcpy(&pStream->cBuf[dwTail], cBuf, dwLen);
	}

	pStream->dwTail = (dwTail+dwLen) % MAX_OUTPUTSTREAM_SIZE;

	*pdwLen = dwLen;
	return 0;
}

/* 
 * ����len���ֽ�
 */ 
int SocketOutputStream_Skip(SocketOutputStream* pStream, uint32_t dwLen) 
{
	if(dwLen == 0)
	{
		return -1;
	}

	if(dwLen > SocketOutputStream_Length(pStream))
	{
		return -2;
	}

	pStream->dwHead = (pStream->dwHead+dwLen) % MAX_OUTPUTSTREAM_SIZE;

	return 0;
}

/*
 * ����
 */
void SocketOutputStream_CleanUp(SocketOutputStream* pStream)
{
	pStream->dwHead = 0;
	pStream->dwTail = 0;
}

/*
 * flush����
 */
int SocketOutputStream_Flush(SocketOutputStream* pStream) 
{
	int nFlushed = 0;
	int nSent = 0;
	int nLeft;

	uint32_t dwHead = pStream->dwHead;
	uint32_t dwTail = pStream->dwTail;

	// ���ñ�־λ����ֹ�����쳣��Ϣ
	if (dwHead < dwTail) 
	{
		nLeft = dwTail - dwHead;

		while (nLeft > 0) 
		{
			int iRet = socketapi_send(pStream->iSocket, &pStream->cBuf[dwHead], nLeft, MSG_NOSIGNAL);
			if(iRet < 0)
			{
				if(errno == EAGAIN || errno == EINTR)
				{
					return 0;
				}
				return -1;
			}
			else if (iRet == 0)
			{
				return 0;
			}

			nFlushed += nSent;
			nLeft -= nSent;
			pStream->dwHead += nSent;
		}
	} 
	else if(dwHead > dwTail) 
	{
		nLeft = MAX_OUTPUTSTREAM_SIZE - dwHead;
		while (nLeft > 0) 
		{
			int iRet = socketapi_send(pStream->iSocket, &pStream->cBuf[dwHead], nLeft, MSG_NOSIGNAL);
			if(iRet < 0)
			{
				if(errno == EAGAIN || errno == EINTR)
				{
					continue;
				}
				return -3;
			}
			else if (iRet == 0)
			{
				return 0;
			}

			nFlushed += nSent;
			nLeft -= nSent;
			pStream->dwHead += nSent;
		}

		pStream->dwHead = 0;

		nLeft = pStream->dwTail;

		while (nLeft > 0) 
		{
			int iRet = socketapi_send(pStream->iSocket, &pStream->cBuf[dwHead], nLeft, MSG_NOSIGNAL);
			if(iRet < 0)
			{
				if(errno == EAGAIN || errno == EINTR)
				{
					continue;
				}
				return -3;
			}

			nFlushed += nSent;
			nLeft -= nSent;
			pStream->dwHead += nSent;
		}
	}

	pStream->dwHead = pStream->dwTail = 0;

	return nFlushed;
}