#include "SocketInputStream.h"
#include "SocketAPI.h"

#define ERR_LENGTH		0xffffffff


/*
 * ��ʼ��, �������˴��룬 ֻ�����һ�γ�ʼ��
 */
int SocketInputStream_Init(SocketInputStream* pStream, int iSocket)
{
	if(pStream == NULL)
	{
		return -1;
	}
	
	memset(pStream, 0, sizeof(SocketInputStream)); 
	pStream->iSocket = iSocket;
	
	return 0;
}

/*
 * ���ض��г���.
 */
uint32_t SocketInputStream_Length(SocketInputStream* pStream)
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
		return (MAX_INPUTSTREAM_SIZE - dwHead + dwTail);
	}

	return ERR_LENGTH;
}

/*
 * ���ض�������
 */
uint32_t SocketInputStream_Capacity(SocketInputStream* pStream)
{
	return MAX_INPUTSTREAM_SIZE; 
}

/*
 * �������Ƿ�Ϊ��
 */
int	SocketInputStream_IsEmpty(SocketInputStream* pStream)
{ 
	return (pStream->dwHead == pStream->dwTail ? 1 : 0); 
}


/*
 * @brief ��ȡlen�ֽڵ�buf
 * @return	0	��ʾû�ж�������
 *			>1	���ض�ȡ���ֽ���
 */
int SocketInputStream_Read(SocketInputStream* pStream, void* cBuf, uint32_t dwLen) 
{
	if(pStream == NULL || cBuf == NULL || dwLen == 0)
	{
		return -1;
	}

	// step 1.�жϵ�ǰ�ɶ�����
	if(dwLen > SocketInputStream_Length(pStream))
	{
		return -2;
	}
	
	uint32_t dwHead = pStream->dwHead;
	uint32_t dwTail = pStream->dwTail;
	// uint32_t dwBufLen = pStream->dwBufLen;

	// step 2. ��ȡ����
	if(dwHead < dwTail) 
	{
		memcpy(cBuf, &pStream->cBuf[dwHead], dwLen);
	} 
	else 
	{
		uint32_t dwRightLen = MAX_INPUTSTREAM_SIZE - dwHead;
		if(dwLen <= dwRightLen) 
		{
			memcpy(cBuf, &pStream->cBuf[dwHead], dwLen);
		} 
		else 
		{
			memcpy(cBuf, &pStream->cBuf[dwHead], dwRightLen);
			memcpy(cBuf+dwRightLen, pStream->cBuf, dwLen-dwRightLen);
		}
	}

	pStream->dwHead = (dwHead+dwLen) % MAX_INPUTSTREAM_SIZE;
	return dwLen;
}

/*
 * ֻ�鿴buf�����ƶ�ָ��
 */
int SocketInputStream_Peek(SocketInputStream* pStream, void* cBuf, uint32_t dwLen) 
{
	if(cBuf == NULL || dwLen == 0)
	{
		return -1;
	}

	// 1. �ж�Ҫ��ȡ�ĳ���
	if(dwLen > SocketInputStream_Length(pStream))
	{
		return -2;
	}

	uint32_t dwHead = pStream->dwHead;
	uint32_t dwTail = pStream->dwTail;

	// 2. 
	if(dwHead < dwTail) 
	{
		memcpy(cBuf, &pStream->cBuf[dwHead], dwLen);
	} 
	else 
	{
		uint32_t dwRightLen = MAX_INPUTSTREAM_SIZE - dwHead;
		if(dwLen <= dwRightLen) 
		{
			memcpy(cBuf, &pStream->cBuf[dwHead], dwLen);
		} 
		else 
		{
			memcpy(cBuf, &pStream->cBuf[dwHead], dwRightLen);
			memcpy(cBuf+dwRightLen, &pStream->cBuf[0], dwLen-dwRightLen);
		}
	}

	return 0;
}

/* 
 * ����len���ֽ�
 */ 
int SocketInputStream_Skip(SocketInputStream* pStream, uint32_t dwLen) 
{
	if(dwLen == 0)
	{
		return -1;
	}

	uint32_t dwHead = pStream->dwHead;
	if(dwLen > SocketInputStream_Length(pStream))
	{
		return -2;
	}

	pStream->dwHead = (dwHead+dwLen) % MAX_INPUTSTREAM_SIZE;

	return 0;
}

/*
 * ��socket��������������
 */
int SocketInputStream_Fill(SocketInputStream* pStream) 
{
	int nFilled = 0;
	int nReceived = 0;
	int nFree = 0;
	
	uint32_t dwHead = pStream->dwHead;
	uint32_t dwTail = pStream->dwTail;
	int iSocket = pStream->iSocket;

	if ( dwHead <= dwTail ) 
	{
		if (dwHead == 0) 
		{
			//---------------------------------------
			// H   T		LEN=10
			// 0123456789
			// abcd......
			//---------------------------------------
			nReceived = 0;

			// step 1: ���Խ���ʣ��Ļ��������������ȵ�����
			nFree = MAX_INPUTSTREAM_SIZE - dwTail - 1;

			if(nFree != 0)
			{
				nReceived = socketapi_recv(iSocket, &pStream->cBuf[dwTail], nFree, 0);
				if (nReceived < 0) 
				{
					if(errno == EAGAIN || errno == EINTR)
					{
						return 0;
					}

					return -11;
				}

				pStream->dwTail += nReceived;
				nFilled += nReceived;
				return nFilled;
			}
		} 
		else 
		{
			// step 3: ֻ����һ�������ݣ���������ʣ�ಿ��

			//
			//    H   T		LEN=10
			// 0123456789
			// ...abcd...
			//
			nFree = MAX_INPUTSTREAM_SIZE - dwTail;
			nReceived = socketapi_recv(iSocket, &pStream->cBuf[dwTail], nFree, 0);
			if(nReceived < 0)
			{
				if(errno == EAGAIN || errno == EINTR)
				{
					return 0;
				}
				return -21;
			}

			pStream->dwTail = (dwTail + nReceived) % MAX_INPUTSTREAM_SIZE;
			nFilled += nReceived;
			return nFilled;
		}
	} 
	else 
	{	
		//
		//     T  H		LEN=10
		// 0123456789
		// abcd...efg
		//

		nReceived = 0;
		nFree = dwHead - dwTail - 1;
		if(nFree != 0)
		{
			nReceived = socketapi_recv(iSocket, &pStream->cBuf[dwTail], nFree, 0);
			if(nReceived < 0)
			{
				if(errno == EAGAIN || errno == EINTR)
				{
					return 0;
				}

				return -31;
			}

			pStream->dwTail += nReceived;
			nFilled += nReceived;
			return nFilled;
		}
	}

	return nFilled;
}

/*
 * ����
 */
void SocketInputStream_CleanUp(SocketInputStream* pStream)
{
	pStream->dwHead = 0;
	pStream->dwTail = 0;
}




