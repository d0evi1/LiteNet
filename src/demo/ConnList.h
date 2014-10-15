/*****************************************************************
 * �ļ�˵�� ConnectionPool.h
 * 
 * @brief		LiteNet�е����������Connection��ɣ�����������̳���.
 *				ConnectionPool��һ���̶����������ӣ���ʼ��ʱȷ����
 *				ÿ��Connection�ϵ�socket��INVALID̬��VALID̬
 *				���þͽ����ò���
 * @author		jolly
 * @date		2012.10.1
 ******************************************************************/
#ifndef _LITE_CONNLIST_H_
#define _LITE_CONNLIST_H_

#include "comm/LiteType.h"
#include "comm/Logger.h"

#define MAX_CONN_NUM	50			// ������

/*
 * ����״̬
 */
typedef enum {
	SOCKET_INIT				=	0,			// close����δ����״̬
	SOCKET_ACTIVE_CONNECT	=	1,			// �������� (����connect���ӵ�)
	SOCKET_PASSIVE_CONNECT	=	2,			// �������� (����accept���ص�)
} CONN_STAT;

/*
 * һ������
 */
typedef struct {
	int				iSocket;				// socket
	int				iServID;				// ����ID
	CONN_STAT		iStat;					// ״̬
	SocketStream	stInputStream;			// input stream
	SocketStream	stOutputStream;			// output stream
	Channel*		pChannel;
} Connection;

/*
 * �����б�������mq�е�ÿ������
 */
typedef struct {
	Logger*			pLogger;			// ��־
	Connection*		pConnections;		// ����
} ConnList;

int				ConnList_Init(ConnList* pList, Logger* pLogger);

Connection*		ConnList_Get(ConnList* pList, int iSocket, );
Connection*		ConnList_Add(ConnList* pList, int iSocket, CONN_STAT iStat);
void			ConnList_Del(ConnList* pList, int iSocket);

#endif