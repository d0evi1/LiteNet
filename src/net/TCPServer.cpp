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
 * 初始化
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

	// step 2. 初始化epoll
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

	// 2、删除epoll
	iRet = Epoller_EventDel(pEpoller, iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "EventDel failed!");
		return -3;
	}
}

/*
 * 关闭连接
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
 * 监听
 */
int TCPServer_Listen(TCPServer* pServer)
{
	// 1、设置非阻塞
	int iRet = socketapi_setnonblocking(pServer->iServSocket, 1);
	if(iRet < 0)
	{
		LOG_ERROR(pServer->pLogger, "SetNoBlock() fail.");
		socketapi_close(pServer->iServSocket);
		return -2;
	}

	// 3、设置地址复用
	iRet = socketapi_setreuse(pServer->iServSocket, 1);
	if(iRet == -1)
	{
		LOG_ERROR(pServer->pLogger, "SetReuseAddr() fail.");
		socketapi_close(pServer->iServSocket);
		return -3;
	}

	// 5、bind()绑定
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

	// 6、listen
	iRet = socketapi_listen(pServer->iServSocket, pServer->iBacklog);
	if(iRet == -1)
	{
		LOG_ERROR(pServer->pLogger, "Listen() fail:iRet=%d,error:%s", iRet, strerror(errno));
		socketapi_close(pServer->iServSocket);
		return -5;
	}

	// 8、EventAdd相应读事件
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
 * 处理Accept
 */
int TCPServer_Accept(TCPServer* pServer)
{
	if(pServer == NULL)
	{
		return -1;
	}

	// step 1: accept接收新连接
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

	// step 2: 置为非阻塞
	int iRet = socketapi_setnonblocking(iSock, 1);
	if(iRet < 0)
	{
		LOG_ERROR(pServer->pLogger, "SetNonBlocking() fail: %d", iRet);
		return -3;
	}

	// step 3: 添加事件
	iRet = Epoller_EventAdd(pServer->pEpoller, iSock, EVENT_READ | EVENT_ERROR);
	if(iRet < 0)
	{
		LOG_ERROR(pServer->pLogger, "EventAdd() fail: %d", iRet);
		socketapi_close(iSock);
		return -4;
	}

	// 额外工作，留出API (不退出程序)
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
 * 每一次接收，
 */
int TCPServer_Input(SOCKET iSocket)
{
	// 1、接收逻辑
	int iRet = OnInput(iSocket);
	if(iRet < 0)
	{
		LOG_ERROR(m_pLogger, "ProcessRead error: iRet = %d", iRet);
		return -2;
	}

	return 0;
}

/*
 * 每一次接收，
 */
int TCPServer_Output(SOCKET iSocket)
{
	// 1、接收逻辑
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
 * Loop循环
 */
int TCPServer_Run(TCPServer* pServer)
{
	// step 1: listen()函数
	int iSocket = TCPServer_Listen(pServer);
	if(iSocket < 0)
	{
		LOG_ERROR(pServer->pLogger, "Listen() fail: iRet=%d", iSocket);
		return -1;
	}

	// step 2: 主循环
	for(;;)
	{
		// a、处理循环timeout逻辑
		OnLoop();

		// b、等待事件到来
		int iActiveNum = Epoller_WaitEvent(pServer->pEpoller, pServer->iTimeOut);
		if(iActiveNum < 0)
		{
			continue;
		}
		
		// c、处理激活事件
		for(int i=0; i<iActiveNum; i++)
		{
			int iFlag = 0;
			int iReadySock = Epoller_GetReadyFd(pServer->pEpoller, i, &iFlag);
			if(iReadySock > 0)
			{
				// 错误事件处理
				if(iFlag & EVENT_ERROR)
				{
					TCPServer_Exception(iReadySock);
					continue;
				}

				// 读事件处理
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
				
				// 写事件处理
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
 * TCP发送包
 * @param pPkg			要发送包的字节数组
 * @param iPkgLen		要发送包的长度
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

	// 边界判断
	int iSendBufSize = sizeof(pContext->cSendBuf);
	if(pPkg == NULL || iPkgLen <= 0 || (iPkgLen > iSendBufSize))
	{
		LOG_ERROR(m_pLogger, "parameter error: %d, %u", iPkgLen, iSendBufSize);
		return -1;
	}

	pContext->tLastTime = time(NULL);

	// 缓冲区
	if(pContext->iBytesToSend > 0)
	{
		// a、缓冲区空间不够
		int iSendBufSize = sizeof(pContext->cSendBuf);
		if( (pContext->iBytesToSend + iPkgLen) > iSendBufSize)
		{
			HandleClose(pContext, pUserInfo);
			return -2;
		}

		// b、添加进缓冲区
		memcpy(pContext->cSendBuf + pContext->iBytesToSend, pPkg, iPkgLen);
		pContext->iBytesToSend += iPkgLen;
		return 0;
	}

	// 发送消息
	iRet = send(pContext->stSocket, pPkg, iPkgLen, 0);
	if(iRet > 0)
	{
		// a、完整发出
		if(iRet == iPkgLen)
		{
			return 0;
		}

		// b、移动发送缓冲区，依靠epoll来发送剩余消息
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

	// 中断
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

	// 出错
	HandleClose(pContext, pUserInfo);
	return -1;
}

#endif