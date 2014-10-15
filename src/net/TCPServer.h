/*
 * TCP服务器实现
 * 使用epoll方式
 *
 * author:  d0evi0
 * date:    2012.6.12
 */

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "comm/LiteType.h"
#include "comm/Logger.h"

#include "EpollBase.h"

#include "Socket.h"
#include "ServSocket.h"

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdint.h>

#define RECVBUF_LEN		(4096*10)
#define SENDBUF_LEN		(4096*10)

typedef struct  
{
	Logger*			pLogger;
	Epoller*		pEpoller;
	int				iServSocket;
	char			szIP[32];
	uint16_t		dwPort;			
	int				iBacklog;
	int				iTimeOut;
	int				iMaxSocketNum;
} TCPServer;




int		TCPServer_Init(Logger* pLogger, const char* szIP, 
					uint16_t wPort, int nBacklog, int iMaxSocketNum, int iTimeoutMS);

int		TCPServer_Run();

int		TCPServer_CloseConnection(SOCKET iSocket);

public:
	virtual int			OnLoop() { return 0; }
	virtual int			OnAccept(SOCKET iSocket) { return 0; }
	virtual int			OnClose(SOCKET iSocket) { return 0; }
	virtual int			OnException(SOCKET iSocket) { return 0; }
	virtual int			OnInput(SOCKET iSocket) { return 0; }
	virtual int			OnOutput(SOCKET iSocket) { return 0; }

protected:
	int			Listen();
	int			HandleAccept();
	int			HandleException(SOCKET iSocket);
	int			HandleInput(SOCKET iSocket);
	int			HandleOutput(SOCKET iSocket);
	int			HandleClose(SOCKET iSocket);
	int			IsServSocket(SOCKET iSocket);
};

#endif
