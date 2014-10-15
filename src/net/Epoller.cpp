#include "Epoller.h"

#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "SocketAPI.h"

/*
 * 工厂函数
 */
Epoller* Epoller_CreateObj(Logger* pLogger, int iMaxSocketNum)
{
	Epoller* pEpoller = new Epoller;
	if(pEpoller == NULL)
	{
		printf("[fatal error]: has no memory for epollbase.\n");
		return NULL;
	}

	int iRet = Epoller_Init(pEpoller, pLogger, iMaxSocketNum);
	if( iRet < 0)
	{
		printf("init epoll base: iRet=%d\n", iRet);
		Epoller_DestroyObj(pEpoller);
		return NULL;
	}

	return pEpoller;
}

/*
 * 销毁对象
 */
void Epoller_DestroyObj(Epoller* pBase)
{
	SAFE_DELETE(pBase);
}


/*
 * Init kernel队列。
 * 创建 32000个socket数，内核2.6.8以后忽略该size(动态)
 */
int	Epoller_Init(Epoller* pEpoller, Logger* pLogger, int iMaxSocketNum)
{
	if(pEpoller == NULL || pLogger == NULL || iMaxSocketNum <= 0)
	{
		return -1;
	}

	pEpoller->pLogger = pLogger;
	pEpoller->iMaxSocketNum = iMaxSocketNum;

	// epfd，创建
	pEpoller->iEpollFd = epoll_create(pEpoller->iMaxSocketNum);
	if (pEpoller->iEpollFd == -1)
	{
		if (errno != ENOSYS)
		{
			LOG_ERROR(pEpoller->pLogger, "epoll_create fail");
		}
		return -2;
	}
	
	// 
	pEpoller->pEvents = new epoll_event[iMaxSocketNum];
	if(pEpoller->pEvents == NULL)
	{
		LOG_ERROR(pEpoller->pLogger, "new epoll_event() fail");
		return -3;
	}

	return 0;
}

/*
 * 根据设置
 */
int Epoller_SetEvent(Epoller* pEpoller, epoll_event* pev, SOCKET iSocket, int iFlag)
{
	if(pEpoller == NULL || pev == NULL || iSocket <= 0)
	{
		return -1;
	}

	int iEvents = 0;

	if(iFlag & EVENT_READ)
	{
		iEvents |= EPOLLIN;		// EPOLLIN|EPOLLET
	}

	if(iFlag & EVENT_WRITE)
	{
		iEvents |= EPOLLOUT;		// EPOLLOUT|EPOLLET
	}
	
	if(iFlag & EVENT_ERROR)
	{
		iEvents |= (EPOLLHUP | EPOLLERR);
	}

	pev->data.fd = iSocket;
	pev->events = iEvents;
	return 0;
}

/*
 * 获取
 */
int Epoller_GetEvent(Epoller* pEpoller, epoll_event* pev, int* piSock, int* piFlag)
{
	if(pev == NULL || piSock == NULL)
	{
		return -1;
	}

	int flag = 0;
	if((pev->events & EPOLLIN) != 0)
	{
		flag |= EVENT_READ;
	}

	if((pev->events & EPOLLOUT) != 0)
	{
		flag |= EVENT_WRITE;
	}

	if((pev->events & (EPOLLHUP|EPOLLERR)) != 0)
	{
		flag |= EVENT_ERROR;
	}

	*piSock = pev->data.fd;
	*piFlag = flag;
	return 0;
}

/*
 * 获取iFlag标记
 */
int Epoller_GetReadyFd(Epoller* pEpoller, int iIndex, int* piFd, int* piFlag)
{
	if(iIndex < 0 || iIndex > pEpoller->iActive)
	{
		return -1;
	}

	int iRet = Epoller_GetEvent(pEpoller, &pEpoller->pEvents[iIndex], piFd, piFlag);
	if(*piFlag == 0 || *piFd < 0)
	{
		return -2;
	}

	return 0;
}

/*
 * 添加事件
 */
int Epoller_EventAdd(Epoller* pEpoller, SOCKET iSocket, int iFlag)
{
	if(iSocket <= 0)
	{
		return -1;
	}

	struct epoll_event stEvent;
	memset(&stEvent, 0, sizeof(stEvent));
	int iRet = Epoller_SetEvent(pEpoller, &stEvent, iSocket, iFlag);
	if(iRet < 0)
	{
		return -2;
	}

	// 
	iRet = epoll_ctl(pEpoller->iEpollFd, EPOLL_CTL_ADD, iSocket, &stEvent);
	if (iRet < 0)
	{	
		// ADD已存在，尝试MOD
		if (errno != EEXIST || epoll_ctl(pEpoller->iEpollFd, EPOLL_CTL_MOD, iSocket, &stEvent) < 0) 
		{
			LOG_ERROR(pEpoller->pLogger, "epoll_ctl add failed: %s", strerror(errno));
			return -1;
		} 
		else 
		{
			LOG_WARN(pEpoller->pLogger, "epoll_ctl add failed.");
		}
	}

	return 0;
}

/*
 * 修改事件
 */
int Epoller_EventMod(Epoller* pEpoller, SOCKET iSocket, int iFlag)
{
	if(iSocket <= 0)
	{
		return -1;
	}

	struct epoll_event stEvent;
	memset(&stEvent, 0, sizeof(stEvent));
	int iRet = Epoller_SetEvent(pEpoller, &stEvent, iSocket, iFlag);
	if(iRet < 0)
	{
		return -2;
	}

	iRet = epoll_ctl(pEpoller->iEpollFd, EPOLL_CTL_MOD, iSocket, &stEvent);
	if (iRet < 0) 
	{
		// ADD已存在，尝试MOD
		if( (errno != ENOENT) && epoll_ctl(pEpoller->iEpollFd, EPOLL_CTL_ADD, iSocket, &stEvent) < 0)
		{
			LOG_ERROR(pEpoller->pLogger, "epoll_ctl MOD fail: %s", strerror(errno));
			return -11;
		} 
		else 
		{
			LOG_WARN(pEpoller->pLogger, "[warn] epoll_ctl mod fail");	
			return -12;
		}
	}

	return 0;
}


/*
 * 删除事件
 */
int Epoller_EventDel(Epoller* pEpoller, int iSocket)
{
	if(iSocket <= 0)
	{
		return -1;
	}

	struct epoll_event stEvent;
	int iRet = Epoller_SetEvent(pEpoller, &stEvent, iSocket, 0);
	if(iRet < 0)
	{
		return -2;
	}

	iRet = epoll_ctl(pEpoller->iEpollFd, EPOLL_CTL_DEL, iSocket, &stEvent);
	if (iRet < 0 && (errno == ENOENT || errno == EBADF ||errno == EPERM) ) 
	{
		LOG_ERROR(pEpoller->pLogger, "epoll_ctl del: %s.", strerror(errno));
		return -3;
	}

	return 0;
}

/*
 * 封装了epoll_wait
 */
int Epoller_WaitEvent(Epoller* pEpoller, int iTimeoutMsec)
{
	int iActive = epoll_wait(pEpoller->iEpollFd, pEpoller->pEvents, pEpoller->iMaxSocketNum, iTimeoutMsec);
	if( iActive < 0 )
	{
		if(errno != EAGAIN && errno != EINTR)
		{
			LOG_ERROR(pEpoller->pLogger, "epoll_wait error!");
		}
		
		return -1;
	}
	
	pEpoller->iActive = iActive;
	return iActive;
}