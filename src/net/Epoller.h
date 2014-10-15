/*****************************************************************
 * Epoller.h 
 * 
 * @brief		ʹ��Epoll ��ˮƽ����ģʽ
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
 * �������͵Ĳ���
 */
typedef enum {
	EVENT_OP_ADD,		// ���
	EVENT_OP_MOD,		// �޸�
	EVENT_OP_DEL,		// ɾ��
} EVENT_OP;

typedef enum {
	EVENT_READ		= 0x1,				// ��
	EVENT_WRITE		= 0x2,				// д
	EVENT_ERROR		= 0x4,				// ������
} EVENT_FLAG;

/*
 * ����epoll�ĺ���
 */
typedef struct {
	int					iEpollFd;					// ��epollfd
	int					iMaxSocketNum;				// ���socket������
	int					iActive;					// һ��wait�������صļ����¼���
	epoll_event*		pEvents;					// һ��wait�������ص�����¼���
	Logger*				pLogger;
} Epoller;

// ��������
Epoller*		Epoller_CreateObj(Logger* pLogger, int iMaxSocketNum);
void			Epoller_DestroyObj(Epoller* pBase);

// ��ʼ��
int		Epoller_Init(Epoller* pEpoll, Logger* pLogger, int iMaxSocketNum);

// �����¼�
int		Epoller_SetEvent(Epoller* pEpoller, epoll_event* pev, int iSocket, int iFlag);
int		Epoller_GetEvent(Epoller* pEpoller, epoll_event* pev, int* piSocket, int* piFlag);

//int		UpdateEvent();

// �ȴ��¼� 
int		Epoller_WaitEvent(Epoller* pEpoller, int iTimeoutMSec);
int		Epoller_GetReadyFd(Epoller* pEpoller, int iIndex, int* piFd, int* piFlag);

// ��ӡ��޸ġ�ɾ���¼�
int		Epoller_EventAdd(Epoller* pEpoller, int iSocket, int iFlag);
int		Epoller_EventMod(Epoller* pEpoller, int iSocket, int iFlag);
int		Epoller_EventDel(Epoller* pEpoller, int iSocket);

#endif