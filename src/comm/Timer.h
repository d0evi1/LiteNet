/*--------------------------------------------------
 * �ο�linux/time.h��ʵ��
 *
 * @author  d0evi1	
 * @date	2012.09.15
 *--------------------------------------------------*/

#ifndef _LITE_TIMER_H_
#define _LITE_TIMER_H_

#include "LiteList.h"

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

/*
 *  ��ʱ����ʱ����ָ��
 */
typedef void (*ExpireFunc)(uint32_t dwDataLen, char* pcData);

/*
 * ��ʱ���ڵ�
 */
typedef struct  
{
	uint32_t		dwSeq;			// seq���Զ��������
	
	ListHead		stList;			// ����
	uint32_t		dwExpire;		// ʲôʱ����
	ExpireFunc		OnExpire;		// ���ں���
	
	uint32_t		dwDataLen;		// �û�����
	char			pcData[0];		// �û�����
} TimerNode;

/*
 * ���е��ڵ����񶼻��ڵ�һ��
 */
#define TVN_BITS		6
#define TVR_BITS		8					// rootʹ��, 2��8�η�=128

#define TVN_SIZE		(1 << TVN_BITS)
#define TVR_SIZE		(1 << TVR_BITS)

#define TVN_MASK		(TVN_SIZE - 1)		// 
#define TVR_MASK		(TVR_SIZE - 1)		// TVRoot�ı�־λ

/*
 * tv1 : ��. 256
 */
typedef struct  
{
	int				iIndex;
	ListHead		stVec[TVR_SIZE];
} TimerVecRoot;

/*
 * tv2, tv3, tv4, tv5 define.
 */
typedef struct  
{
	int			iIndex;
	ListHead	stVec[TVN_SIZE];
}TimerVec;

#define TVECS_SIZE	5


typedef struct  
{
	uint32_t		dwCheckTime;				// ��ǰ��checkʱ�䣬Ϊ�˷�����checktime������޸�
	uint32_t		dwCheckIntervalUs;		// 
	uint32_t		dwNodeCount;
	uint32_t		dwMaxNodeCount;
	uint32_t		dwMaxLen;
	struct timeval	stBaseTV;					// ��ʱ������ʱ��

	// ��ʱ���б����
	TimerVec*			tvecs[TVECS_SIZE];
	TimerVecRoot		tv1;
	TimerVec			tv2;
	TimerVec			tv3;
	TimerVec			tv4;
	TimerVec			tv5;
} Timer;



int Timer_Init(Timer* pTimer, uint32_t dwIntervalUs);

int Timer_Add(Timer* pTimer, uint32_t dwTimeout, TimerNode* pstNode);

//int Timer_Mod(Timer* pTimer, uint32_t dwTimeout);
//int Timer_Get(Timer* pTimer, uint32_t dwSeq, uint32_t* pdwDataLen, char** ppcData);

void Timer_Check(Timer* pTimer);


#endif
