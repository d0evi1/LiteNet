/*
 * 预留 <= 一个包的长度
 * Head用于预取， 在尾部分开情况(简单)
 */


#ifndef _CNANNEL_H_
#define _CNANNEL_H_

#include "LiteType.h"
#include <stdint.h>

#pragma pack(1)


//#define CHANNEL_BUFFER_LENGTH		(2*1000*1000)
#define CHANNEL_BUF_LEN		10000

typedef struct
{
	char				cFlag;		// 包标记(PSTA 或 PEND)
	uint16_t			wLength;	// 只是后面包体的长度
} ChannelPkgHead;

/* 
 * 队列
 */
typedef struct  
{
	uint32_t		dwTotalPkgs;
	uint32_t		dwHead;		
	uint32_t		dwTail;		 
	char			cBuf[CHANNEL_BUF_LEN];
} RingQueue;

typedef struct {
	RingQueue*		pReqQ;
	RingQueue*		pRspQ;
} Channel;

int	Channel_Init(Channel* pChannel, uint32_t iReqKey, uint32_t iRspKey);

int	Channel_AddPkg(Channel* pChannel, void *pBody, int iBodyLen,  int iIsRequest);
	
int	Channel_GetPkg(Channel* pChannel, void *pBody, int *piBodyLen, int iIsRequest);
	
int	Channel_GetPkgNum(Channel* pChannel, int iIsRequest);
	
#pragma pack()

#endif