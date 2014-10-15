/*--------------------------------------------------
 * 参考linux/time.h的实现
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
 *  定时器超时函数指针
 */
typedef void (*ExpireFunc)(uint32_t dwDataLen, char* pcData);

/*
 * 定时器节点
 */
typedef struct  
{
	uint32_t		dwSeq;			// seq，自动随机生成
	
	ListHead		stList;			// 链表
	uint32_t		dwExpire;		// 什么时候到期
	ExpireFunc		OnExpire;		// 到期函数
	
	uint32_t		dwDataLen;		// 用户数据
	char			pcData[0];		// 用户数据
} TimerNode;

/*
 * 所有到期的任务都会在第一组
 */
#define TVN_BITS		6
#define TVR_BITS		8					// root使用, 2的8次方=128

#define TVN_SIZE		(1 << TVN_BITS)
#define TVR_SIZE		(1 << TVR_BITS)

#define TVN_MASK		(TVN_SIZE - 1)		// 
#define TVR_MASK		(TVR_SIZE - 1)		// TVRoot的标志位

/*
 * tv1 : 根. 256
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
	uint32_t		dwCheckTime;				// 当前已check时间，为了方便在checktime后继续修改
	uint32_t		dwCheckIntervalUs;		// 
	uint32_t		dwNodeCount;
	uint32_t		dwMaxNodeCount;
	uint32_t		dwMaxLen;
	struct timeval	stBaseTV;					// 定时器创建时间

	// 定时器列表相关
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
