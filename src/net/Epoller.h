/*****************************************************************
 * Epoller.h 
 * 
 * @brief		使用Epoll 的水平触发模式
 * @author		jolly
 * @date		2012.
 ******************************************************************/

#ifndef _LITE_EPOLLER_H_
#define _LITE_EPOLLER_H_

#include "comm/LiteType.h"
#include "comm/Logger.h"

#include <sys/epoll.h>

#define MAX_EVENT_NUM		1
#define MAX_BUF_SIZE		4096*10		// 40k

/*
 * 三种类型的操作
 */
typedef enum {
	EVENT_OP_ADD,		// 添加
	EVENT_OP_MOD,		// 修改
	EVENT_OP_DEL,		// 删除
} EVENT_OP;

typedef enum {
	EVENT_READ		= 0x1,				// 读
	EVENT_WRITE		= 0x2,				// 写
	EVENT_ERROR		= 0x4,				// 错误处理
} EVENT_FLAG;

/*
 * 操纵epoll的函数
 */
typedef struct {
	int					iEpollFd;					// 主epollfd
	int					iMaxSocketNum;				// 最大socket连接数
	int					iActive;					// 一次wait操作返回的激活事件数
	epoll_event*		pEvents;					// 一次wait操作返回的最多事件数
	Logger*				pLogger;
} Epoller;

// 工厂函数
Epoller*		Epoller_CreateObj(Logger* pLogger, int iMaxSocketNum);
void			Epoller_DestroyObj(Epoller* pBase);

// 初始化
int		Epoller_Init(Epoller* pEpoll, Logger* pLogger, int iMaxSocketNum);

// 设置事件
int		Epoller_SetEvent(Epoller* pEpoller, epoll_event* pev, int iSocket, int iFlag);
int		Epoller_GetEvent(Epoller* pEpoller, epoll_event* pev, int* piSocket, int* piFlag);

//int		UpdateEvent();

// 等待事件 
int		Epoller_WaitEvent(Epoller* pEpoller, int iTimeoutMSec);
int		Epoller_GetReadyFd(Epoller* pEpoller, int iIndex, int* piFd, int* piFlag);

// 添加、修改、删除事件
int		Epoller_EventAdd(Epoller* pEpoller, int iSocket, int iFlag);
int		Epoller_EventMod(Epoller* pEpoller, int iSocket, int iFlag);
int		Epoller_EventDel(Epoller* pEpoller, int iSocket);

#endif