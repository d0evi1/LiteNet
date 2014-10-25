#include "ConnectionPool.h"

#include "comm/Logger.h"

/*
 * 初始化
 */
int ConnList_Init(ConnList* pList, Logger* pLogger)
{
	if(pList == NULL)
	{
		return -1;
	}
	
	pList->pLogger = pLogger;
	pList->pConnections = new Connection[MAX_CONN_NUM];
	if(pList->pConnections == NULL)
	{
		XLOG_ERROR(pList->pLogger, "ConnectionPool: new() fail.");
		return -2;
	}

	memset(pList->pConnections, 0, sizeof(Connection)*MAX_CONN_NUM);
	
	return 0;
}

/*
 * 设置连接
 */
Connection*	ConnList_Add(ConnList* pList, int iSocket, CONN_STAT iStat)
{
	if(pList == NULL || iSocket <= 0)
	{
		return NULL;
	}

	for(int i = 0; i < MAX_CONN_NUM; i++)
	{
		if( pList->pConnections[i].iStat == SOCKET_INIT)
		{
			pList->pConnections[i].iSocket = iSocket;
			pList->pConnections[i].iStat = iStat;
			return &(pList->pConnections[i]);
		}
	}

	// 表示满
	XLOG_WARN(pList->pLogger, "ConnectionPool is Full.");
	return NULL;
}

/*
 * 获取连接
 */
Connection*	ConnList_Get(ConnList* pList, int iSocket)
{
	if(pList == NULL || iSocket <= 0)
	{
		return NULL;
	}

	for(int i=0; i<MAX_CONN_NUM; i++)
	{
		if(pList->pConnections[i].iSocket == iSocket)
		{
			return &(pList->pConnections[i]);
		}
	}

	XLOG_WARN(pList->pLogger, "not exist socket.");
	return NULL;
}

/*
 * 删除连接
 */
void ConnList_Del(ConnList* pList, int iSocket)
{
	Connection* pConnection = ConnList_Get(pList, iSocket);
	if(pConnection == NULL)
	{
		return;
	}

	// 清空
	pConnection->iSocket = -1;
	pConnection->iStat = SOCKET_INIT;
}
