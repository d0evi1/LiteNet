#include "TCPServer.h"

#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

#include "SocketAPI.h"

/*
 * ��ʼ��
 */
int	TCPServer::InitServer(Logger* pLogger, const char* szIP, uint16_t wPort, int nBacklog, 
					int iMaxSocketNum, int iTimeoutMS)
{
	if(szIP == NULL)
	{
		return -1;
	}
	
	// step 1: init server socket.
	m_pServSocket = new ServSocket;
	if(m_pServSocket == NULL)
	{
		return -11;
	}

	int iRet = m_pServSocket->Init(szIP, wPort, nBacklog);
	if(iRet < 0)
	{
		return -12;
	}

	// step 2. ��ʼ��epoll
	m_pEpollBase = new EpollBase;
	if(m_pEpollBase == NULL)
	{
		return -4;
	}

	iRet = m_pEpollBase->Init(pLogger, iMaxSocketNum);
	if( iRet < 0)
	{
		return -5;
	}

	m_iMaxSocketNum = iMaxSocketNum;
	m_iTimeoutMS = iTimeoutMS;
	m_pLogger = pLogger;
	m_iInit = 1;

	return 0;
}

int TCPServer::CloseConnection(SOCKET iSocket)
{
	if(iSocket <= 0)
	{
		return -1;
	}

	int iRet = SocketAPI::Close(iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "Close() failed!");
		return -2;
	}

	// 2��ɾ��epoll
	iRet = Epoller_EventDel(pEpoller, iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "EventDel failed!");
		return -3;
	}
}

/*
 * �ر�����
 */
int TCPServer::HandleClose(SOCKET iSocket)
{

	int iRet = OnClose(iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "OnClose failed: iRet=%d", iRet);
	}

	iRet = CloseConnection(iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "Close() failed!");
		return -2;
	}

	return 0;
}

/*
 * ����
 */
int TCPServer_Listen(TCPServer* pServer)
{
	// 1�����÷�����
	int iRet = socketapi_setnonblocking(pServer->iServSocket, 1);
	if(iRet < 0)
	{
		LOG_ERROR(pServer->pLogger, "SetNoBlock() fail.");
		socketapi_close(pServer->iServSocket);
		return -2;
	}

	// 3�����õ�ַ����
	iRet = socketapi_setreuse(pServer->iServSocket, 1);
	if(iRet == -1)
	{
		LOG_ERROR(pServer->pLogger, "SetReuseAddr() fail.");
		socketapi_close(pServer->iServSocket);
		return -3;
	}

	// 5��bind()��
	sockaddr_in stServAddr;
	memset(&stServAddr, 0, sizeof(stServAddr));
	stServAddr.sa_family = AF_INET;
	stServAddr.sin_port = htons(pServer->dwPort);
	stServAddr.sin_addr.s_addr = inet_addr(pServer->szIP);
	
	int iSize = sizeof(stServAddr);
	iRet = socketapi_bind(pServer->iServSocket, &stServAddr, iSize);
	if(iRet == -1)
	{
		LOG_ERROR(pServer->pLogger, "Bind() fail: error:%s", strerror(errno));
		socketapi_close(pServer->iServSocket);
		return -4;
	}

	// 6��listen
	iRet = socketapi_listen(pServer->iServSocket, pServer->iBacklog);
	if(iRet == -1)
	{
		LOG_ERROR(pServer->pLogger, "Listen() fail:iRet=%d,error:%s", iRet, strerror(errno));
		socketapi_close(pServer->iServSocket);
		return -5;
	}

	// 8��EventAdd��Ӧ���¼�
	iRet = Epoller_EventAdd(pServer->iServSocket, EVENT_READ|EVENT_ERROR);
	if(iRet < 0)
	{
		LOG_ERROR(pServer->pLogger, "EventAdd failed!");
		socketapi_close(pServer->iServSocket);
		return -6;
	}

	return 0;
}

/*
 * ����Accept
 */
int TCPServer_Accept(TCPServer* pServer)
{
	if(pServer == NULL)
	{
		return -1;
	}

	// step 1: accept����������
	sockaddr_in stAddr;
	memset(&stAddr, 0, sizeof(stAddr));
	int iLen = sizeof(stAddr);
	int iSock = socketapi_accept(pServer->iServSocket, (struct sockaddr*)&stAddr, (socklen_t*)&iLen);
	if(iSock == -1)
	{
		if(errno == EAGAIN || errno == EINTR)
		{
			return 0;
		}

		LOG_ERROR(pServer->pLogger, "[error] accept error: (%s).", strerror(errno));
		return -2;
	}

	// step 2: ��Ϊ������
	int iRet = socketapi_setnonblocking(iSock, 1);
	if(iRet < 0)
	{
		LOG_ERROR(pServer->pLogger, "SetNonBlocking() fail: %d", iRet);
		return -3;
	}

	// step 3: ����¼�
	iRet = Epoller_EventAdd(pServer->pEpoller, iSock, EVENT_READ | EVENT_ERROR);
	if(iRet < 0)
	{
		LOG_ERROR(pServer->pLogger, "EventAdd() fail: %d", iRet);
		socketapi_close(iSock);
		return -4;
	}

	// ���⹤��������API (���˳�����)
#if 0
	iRet = OnAccept(iSock);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "OnAccept() fail: %d", iRet);
	}
#endif

	return 0;
}

/*
 * ÿһ�ν��գ�
 */
int TCPServer_Input(SOCKET iSocket)
{
	// 1�������߼�
	int iRet = OnInput(iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "ProcessRead error: iRet = %d", iRet);
		return -2;
	}

	return 0;
}

/*
 * ÿһ�ν��գ�
 */
int TCPServer_Output(SOCKET iSocket)
{
	// 1�������߼�
	int iRet = OnOutput(iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "OnOutput error: iRet = %d", iRet);
		return -2;
	}

	return 0;
}

int	TCPServer_Exception(SOCKET iSocket)
{
	if(iSocket <= 0)
	{
		return -1;
	}

	int iRet = OnException(iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "OnException Error: iRet = %d", iRet);
	}

	return 0;
}

/*
 * Loopѭ��
 */
int TCPServer_Run(TCPServer* pServer)
{
	// step 1: listen()����
	int iSocket = TCPServer_Listen(pServer);
	if(iSocket < 0)
	{
		LOG_ERROR(pServer->pLogger, "Listen() fail: iRet=%d", iSocket);
		return -1;
	}

	// step 2: ��ѭ��
	for(;;)
	{
		// a������ѭ��timeout�߼�
		OnLoop();

		// b���ȴ��¼�����
		int iActiveNum = Epoller_WaitEvent(pServer->pEpoller, pServer->iTimeOut);
		if(iActiveNum < 0)
		{
			continue;
		}
		
		// c���������¼�
		for(int i=0; i<iActiveNum; i++)
		{
			int iFlag = 0;
			int iReadySock = Epoller_GetReadyFd(pServer->pEpoller, i, &iFlag);
			if(iReadySock > 0)
			{
				// �����¼�����
				if(iFlag & EVENT_ERROR)
				{
					TCPServer_Exception(iReadySock);
					continue;
				}

				// ���¼�����
				if(iFlag & EVENT_READ)
				{
					if(iReadySock == pServer->iServSocket)
					{
						TCPServer_Accept(pServer);
					}
					else
					{
						TCPServer_Input(pServer, iReadySock);
					}
					continue;
				}
				
				// д�¼�����
				if(iFlag & EVENT_WRITE)
				{
					TCPServer_Output(pServer);
					continue;
				}
			}
		}
	}
}


#if 0
/*
 * TCP���Ͱ�
 * @param pPkg			Ҫ���Ͱ����ֽ�����
 * @param iPkgLen		Ҫ���Ͱ��ĳ���
 */
int TCPServer::TCPSend(SocketContext* pstSctx, char* pUserInfo, char* pPkg, int iPkgLen)
{
	int iRet = 0;
	SocketContext* pContext = (SocketContext*) pstSctx;

	if(!pContext)
	{
		return -1;
	}

	if(pContext->iStat != SOCKET_ACCEPT)
	{
		if(pContext->iStat != SOCKET_INIT)
		{
			LOG_ERROR(m_pLogger, "[error] stat (socket %d, stat %d)!", 
						pContext->stSocket, pContext->iStat);
			return -1;
		}
		return 0;
	}

	// �߽��ж�
	int iSendBufSize = sizeof(pContext->cSendBuf);
	if(pPkg == NULL || iPkgLen <= 0 || (iPkgLen > iSendBufSize))
	{
		LOG_ERROR(m_pLogger, "parameter error: %d, %u", iPkgLen, iSendBufSize);
		return -1;
	}

	pContext->tLastTime = time(NULL);

	// ������
	if(pContext->iBytesToSend > 0)
	{
		// a���������ռ䲻��
		int iSendBufSize = sizeof(pContext->cSendBuf);
		if( (pContext->iBytesToSend + iPkgLen) > iSendBufSize)
		{
			HandleClose(pContext, pUserInfo);
			return -2;
		}

		// b����ӽ�������
		memcpy(pContext->cSendBuf + pContext->iBytesToSend, pPkg, iPkgLen);
		pContext->iBytesToSend += iPkgLen;
		return 0;
	}

	// ������Ϣ
	iRet = send(pContext->stSocket, pPkg, iPkgLen, 0);
	if(iRet > 0)
	{
		// a����������
		if(iRet == iPkgLen)
		{
			return 0;
		}

		// b���ƶ����ͻ�����������epoll������ʣ����Ϣ
		memcpy(pContext->cSendBuf, pPkg+iRet, iPkgLen-iRet);
		pContext->iBytesToSend = iPkgLen - iRet;
		iRet = m_pEpollBase->EventMod(pContext->stSocket, EVENT_READ | EVENT_WRITE | EVENT_ERROR);
		if(iRet < 0)
		{
			HandleClose(pContext, pUserInfo);
			return -3;
		}
		return 0;
	}

	// �ж�
	if(iRet < 0 && (errno == EAGAIN || errno == EINTR))
	{
		memcpy(pContext->cSendBuf, pPkg, iPkgLen);
		pContext->iBytesToSend = iPkgLen;
		iRet = m_pEpollBase->EventMod(pContext->stSocket, EVENT_READ | EVENT_WRITE | EVENT_ERROR);
		if(iRet < 0)
		{
			HandleClose(pContext, pUserInfo);
			return -4;
		}

		return 0;
	}

	// ����
	HandleClose(pContext, pUserInfo);
	return -1;
}

#endif