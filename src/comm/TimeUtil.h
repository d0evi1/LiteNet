/*****************************************************************
 * �ļ�˵�� 
 * 
 * @brief		һЩ���õ�ʱ�亯������
 * @author		jolly
 * @date		2012.7.13
 ******************************************************************/
#ifndef _LITE_TIMEUTIL_H_
#define _LITE_TIMEUTIL_H_

#include "LiteType.h"


int TimeUtil_GetCurDate(char* szCurDate, uint32_t iSize);
int TimeUtil_GetCurDate2(char* szCurDate, uint32_t iSize);

int TimeUtil_GetCurDateTime(char* szCurDateTime, uint32_t iSize);
int TimeUtil_GetCurDateTime2(char* szCurDateTime, uint32_t iSize);

int TimeUtil_GetExactCurDateTime(char* szCurDateTime, uint32_t iSize);
int TimeUtil_GetExactCurDateTime2(char* szCurDateTime, uint32_t iSize);

int TimeUtil_GetTime(char* szCurTime, uint32_t iSize);
int TimeUtil_GetTime2(char* szCurTime, uint32_t iSize);



#endif