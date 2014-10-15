/*****************************************************************
 * 文件说明 ConnectionPool.h
 * 
 * @brief		LiteNet中的网络对象，由Connection组成，其它对象均继承它.
 *				ConnectionPool是一个固定数量的连接，初始化时确定，
 *				每个Connection上的socket有INVALID态、VALID态
 *				采用就近设置策略
 * @author		jolly
 * @date		2012.10.1
 ******************************************************************/
#ifndef _LITE_CONNLIST_H_
#define _LITE_CONNLIST_H_

#include "comm/LiteType.h"
#include "comm/Logger.h"

#define MAX_CONN_NUM	50			// 连接数

/*
 * 连接状态
 */
typedef enum {
	SOCKET_INIT				=	0,			// close或者未连接状态
	SOCKET_ACTIVE_CONNECT	=	1,			// 主动连接 (调用connect连接的)
	SOCKET_PASSIVE_CONNECT	=	2,			// 被动连接 (调用accept返回的)
} CONN_STAT;

/*
 * 一个连接
 */
typedef struct {
	int				iSocket;				// socket
	int				iServID;				// 服务ID
	CONN_STAT		iStat;					// 状态
	SocketStream	stInputStream;			// input stream
	SocketStream	stOutputStream;			// output stream
	Channel*		pChannel;
} Connection;

/*
 * 连接列表，管理着mq中的每个连接
 */
typedef struct {
	Logger*			pLogger;			// 日志
	Connection*		pConnections;		// 连接
} ConnList;

int				ConnList_Init(ConnList* pList, Logger* pLogger);

Connection*		ConnList_Get(ConnList* pList, int iSocket, );
Connection*		ConnList_Add(ConnList* pList, int iSocket, CONN_STAT iStat);
void			ConnList_Del(ConnList* pList, int iSocket);

#endif