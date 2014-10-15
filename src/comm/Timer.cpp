#include "Timer.h"

#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

/*
 * tv1 > tv2.
 * ����ʱ������us��
 */
#define TIME_DIFF_US(tv1,tv2)	(1000000 * (uint64_t)((tv1).tv_sec -(tv2).tv_sec) \
								+(tv1).tv_usec - (tv2).tv_usec)



/*
 * ��ʼ��timer.
 */
int Timer_Init(Timer* pTimer, uint32_t dwCheckIntervalUs)
{
	if(pTimer == NULL)
	{
		return -1;
	}

	memset(pTimer, 0, sizeof(Timer));

	pTimer->tvecs[0] = (TimerVec*)&pTimer->tv1;
	pTimer->tvecs[1] = &pTimer->tv2;
	pTimer->tvecs[2] = &pTimer->tv3;
	pTimer->tvecs[3] = &pTimer->tv4;
	pTimer->tvecs[4] = &pTimer->tv5;

	// ��ʼ��n��
	int i;
	for(i=0; i < TVN_SIZE; i++)
	{
		INIT_LIST_HEAD(pTimer->tv5.stVec + i);
		INIT_LIST_HEAD(pTimer->tv4.stVec + i);
		INIT_LIST_HEAD(pTimer->tv3.stVec + i);
		INIT_LIST_HEAD(pTimer->tv2.stVec + i);
	}

	pTimer->tv5.iIndex = 0;
	pTimer->tv4.iIndex = 0;
	pTimer->tv3.iIndex = 0;
	pTimer->tv2.iIndex = 0;

	// ��ʼ����һ��
	for(i=0; i<TVR_SIZE; i++) 
	{
		INIT_LIST_HEAD(pTimer->tv1.stVec + i);
	}
	pTimer->tv1.iIndex = 0;

	struct timeval stTV;
	pTimer->dwCheckIntervalUs = dwCheckIntervalUs;

	if(pTimer->dwCheckIntervalUs <= 0)
	{
		return -2;
	}

	gettimeofday(&stTV, NULL);
	pTimer->stBaseTV = stTV;
	pTimer->dwCheckTime = 0;
	return 0;
}

/*
 * �ڲ�����
 * ��鶨ʱ��
 */
inline int Timer_InternalAdd(Timer* pTimer, TimerNode* pstNode)
{
	if(pTimer == NULL || pstNode == NULL)
	{
		return -1;
	}

	uint32_t expires = pstNode->dwExpire;

	//����ʱ����ѯλ���ĸ�λ��, ����vec.
	int idx = expires - pTimer->dwCheckTime;
	ListHead* vec = NULL;
	int i = 0;
	if( idx < 0 )
	{
		// С��0������tv1.
		vec = pTimer->tv1.stVec + pTimer->tv1.iIndex;
	}
	else if(idx < TVR_SIZE)
	{
		// ����tv1��
		//printf("=====tv1\n");
		i = (int)(expires & TVR_MASK);
		vec = pTimer->tv1.stVec + i;
	}
	else if( idx < (1 << (TVR_BITS+TVN_BITS))  )
	{
		// ����tv2��
		//printf("=====tv2\n");
		i = (int)((expires >> TVR_BITS) & TVR_MASK);
		vec = pTimer->tv2.stVec + i;
	}
	else if( idx < (1 << (TVR_BITS + 2 * TVN_BITS)) )
	{
		// ����tv3��
		//printf("=====tv3\n");
		i = (int)((expires >> (TVR_BITS+TVN_BITS)) & TVN_MASK);
		vec = pTimer->tv3.stVec + i;
	}
	else if( idx < (1 << (TVR_BITS + 3 * TVN_BITS)) )
	{
		// ����tv4��
		//printf("=====tv4\n");
		i = (int)((expires >> (TVR_BITS + 2*TVN_BITS)) & TVN_MASK);
		vec = pTimer->tv4.stVec + i;
	}
	else
	{
		// ����tv5��
		//printf("=====tv5\n");
		i = (int)((expires >> (TVR_BITS + 3*TVN_BITS)) & TVN_MASK);
		vec = pTimer->tv5.stVec + i;
	}

	//printf("[debug] idex=%d, i=%d,vec-address=%x, node-address=%x\n", idx, i, vec, pstNode);
	ListAdd(&(pstNode->stList), vec->prev);
	//printf("vec index = %d\n", i);
	return 0;
}


/*
 * ����һ���еĵ��ڽڵ㣬Ų��root����
 * ͬ��: ���ڶ��ס������ס����Ľ�Ų����һ��
 */
inline int Timer_CascadeTimers(Timer* pTimer, TimerVec* tv)
{
	if(tv == NULL)
	{
		return -1;
	}

	ListHead* pstHead = NULL;
	ListHead* pstCur = NULL;
	ListHead* pstNext = NULL;

	// 1����ȡtv�������������ָ����һλ��
	pstHead = tv->stVec + tv->iIndex;
	pstCur = pstHead->next;

	// 2������λ�ýڵ���ӵ���ʱ����
	while(pstCur != pstHead)
	{
		TimerNode* pstCurNode = LIST_ENTRY(pstCur, TimerNode, stList);
		ListHead* pstNext = pstCur->next;
		Timer_InternalAdd(pTimer, pstCurNode);
		pstCur = pstNext;
	}

	// 3��һ��ȫ��ɾ��
	INIT_LIST_HEAD(pstHead);
	tv->iIndex = (tv->iIndex + 1) & TVN_MASK;

	return 0;
}

/*
 * 
 */
int Timer_Add(Timer* pTimer, uint32_t dwTimeOut, TimerNode* pstNode)
{
	struct timeval stTV;

	if( pTimer == NULL || pTimer->dwCheckIntervalUs <= 0)
	{
		return -1;
	}
	
	// 
	gettimeofday(&stTV, NULL);
	pstNode->dwExpire = (uint32_t)(TIME_DIFF_US(stTV, pTimer->stBaseTV) / pTimer->dwCheckIntervalUs + dwTimeOut);

	int iRet = Timer_InternalAdd(pTimer, pstNode);
	if(iRet < 0)
	{
		return -2;
	}

	pTimer->dwNodeCount++;
	if(pTimer->dwNodeCount > pTimer->dwMaxNodeCount)
	{
		pTimer->dwMaxNodeCount = pTimer->dwNodeCount;
	}

	if(pstNode->dwDataLen > pTimer->dwMaxLen)
	{
		pTimer->dwMaxLen = pstNode->dwDataLen;
	}

	return 0;
}




/*
 * ���ʱ���б�
 */
inline void Timer_RunTimerList(Timer* pTimer, uint32_t dwCurTime)
{
	while(dwCurTime >= pTimer->dwCheckTime) 
	{
		if(pTimer->tv1.iIndex == 0)
		{
			int n=1;
			do 
			{
				Timer_CascadeTimers(pTimer, pTimer->tvecs[n]);
			} while( (pTimer->tvecs[n]->iIndex == 1) && (++n < TVECS_SIZE) );
		}

		// ��ȡͷ�ڵ㣬����ɨ�裬 �����Ƴ���һ�ڵ�
		ListHead* pstHead = pTimer->tv1.stVec + pTimer->tv1.iIndex;
		ListHead* pstCur = pstHead->next;

		while(pstCur != pstHead)
		{
			//printf("[debug] head index = %d, head=%x, cur=%x\n", pTimer->tv1.iIndex, pstHead, pstCur);
			//printf("[debug] prev=%x, next=%x\n", pstCur->prev, pstCur->next);
			TimerNode* pstCurNode = LIST_ENTRY(pstCur, TimerNode, stList);
			// printf("[debug] pstCurNode=%x\n", pstCurNode);
			if(pstCurNode == NULL)
			{
				printf("bug: %s %d\n", __FILE__, __LINE__);
			}
			
			uint32_t dwDataLen = pstCurNode->dwDataLen;
			char* pcData = pstCurNode->pcData;
			
			if(pstCurNode->OnExpire)
			{
				//printf("[debug] func address:%x\n", pstCurNode->OnExpire);
				pstCurNode->OnExpire(dwDataLen, pcData);
			}

			ListDel(pstCur);
			pTimer->dwNodeCount--;
			memset(pstCurNode, 0, sizeof(TimerNode));

			// ��һ��ѭ��
			pstHead = pTimer->tv1.stVec + pTimer->tv1.iIndex;
			pstCur = pstHead->next;
		}

		// next loop, ��������
		++pTimer->dwCheckTime;
		pTimer->tv1.iIndex = (pTimer->tv1.iIndex + 1) & TVR_MASK;
	}
}

/*
 * ��鶨ʱ����ǰʱ��
 */
void Timer_Check(Timer* pTimer)
{
	uint32_t dwCurTime;
	struct timeval stTV;

	if(pTimer->dwCheckIntervalUs <= 0)
	{
		return;
	}

	// ��ȡ��ǰʱ�䣬�Ƚ��Ƿ�interval.
	gettimeofday(&stTV, NULL);
	dwCurTime = (uint32_t) (TIME_DIFF_US(stTV,pTimer->stBaseTV)/pTimer->dwCheckIntervalUs);

	// ��ʱʱ�䳬��check_time. ���ʱ������
	if((uint32_t) (dwCurTime - pTimer->dwCheckTime) >= 0)
	{
		//printf("[debug]: dwCurTime=%u, pTimer->dwCheckTime=%u\n", dwCurTime, pTimer->dwCheckTime);
		Timer_RunTimerList(pTimer, dwCurTime);
	}
}

#if 0
int GetTimer(uint32_t dwSeq, uint32_t* pdwDataLen, char* psData)
{
	TimerNode* pstNode;
	TimerNode stNode;

	if( 0 >= dwCheckInvlUs)
	{
		return -1;
	}

	if(pdwDataLen == NULL || psData == NULL)
	{
		return -2;
	}

	stNode.dwSeq = dwSeq;
	if(dwTimerType != NON_CONST_DATA_LEN_TIMER)
	{
		// find hash. return stNode.
		
		*pdwDataLen = pstNode->dwDataLen;
		*psData = pstNode->pcData;
	}
}

#endif





