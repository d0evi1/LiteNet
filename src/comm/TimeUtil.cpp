#include "TimeUtil.h"

#include <time.h>
#include <stdio.h>
#include <sys/time.h>


/*
 *
 */
int TimeUtil_GetCurDate(char* szCurDate, uint32_t iSize)
{
	if(szCurDate == NULL)
	{
		return -1;
	}

	time_t stCurTime = time(NULL);
	struct tm * stCurTM = localtime(&stCurTime);

	if( stCurTM->tm_year > 50 )
	{
		snprintf(szCurDate, iSize, "%04d-%02d-%02d", stCurTM->tm_year+1900, 
								stCurTM->tm_mon+1, stCurTM->tm_mday);
	}
	else
	{
		snprintf(szCurDate, iSize, "%04d-%02d-%02d", stCurTM->tm_year+2000, 
								stCurTM->tm_mon+1, stCurTM->tm_mday);
	}

	return 0;
}

/*
 * 
 */
int TimeUtil_GetCurDate2(char* szCurDate, uint32_t iSize)
{
	if(szCurDate == NULL)
	{
		return -1;
	}

	time_t stCurTime = time(NULL);
	struct tm * stCurTM = localtime(&stCurTime);

	if( stCurTM->tm_year > 50 )
	{
		snprintf(szCurDate, iSize, "%04d%02d%02d", stCurTM->tm_year+1900, 
								stCurTM->tm_mon+1, stCurTM->tm_mday);
	}
	else
	{
		snprintf(szCurDate, iSize, "%04d%02d%02d", stCurTM->tm_year+2000, 
								stCurTM->tm_mon+1, stCurTM->tm_mday);
	}

	return 0;
}

/*
 *
 */
int TimeUtil_GetCurDateTime(char* szCurDateTime, uint32_t iSize)
{
	if(szCurDateTime == NULL)
	{
		return -1;
	}
		
	time_t stCurTime = time(NULL);
	struct tm * stCurTM = localtime(&stCurTime);

	if(stCurTM->tm_year > 50)
	{
		snprintf(szCurDateTime, iSize, "%04d-%02d-%02d %02d:%02d:%02d",
				stCurTM->tm_year+1900, stCurTM->tm_mon+1, stCurTM->tm_mday,
				stCurTM->tm_hour, stCurTM->tm_min, stCurTM->tm_sec);
	}
	else
	{
		snprintf(szCurDateTime, iSize, "%04d-%02d-%02d %02d:%02d:%02d", 
				stCurTM->tm_year+2000, stCurTM->tm_mon+1, stCurTM->tm_mday,
				stCurTM->tm_hour, stCurTM->tm_min, stCurTM->tm_sec);
	}

	return 0;
}

/*
 *
 */
int TimeUtil_GetCurDateTime2(char* szCurDateTime, uint32_t iSize)
{
	if(szCurDateTime == NULL)
	{
		return -1;
	}
	
	time_t stCurTime = time(NULL);
	struct tm * stCurTM = localtime(&stCurTime);

	if( stCurTM->tm_year > 50 )
	{
		snprintf(szCurDateTime, iSize, "%04d%02d%02d %02d:%02d:%02d",
				stCurTM->tm_year+1900, stCurTM->tm_mon+1, stCurTM->tm_mday,
				stCurTM->tm_hour, stCurTM->tm_min, stCurTM->tm_sec);
	}
	else
	{
		snprintf(szCurDateTime, iSize, "%04d%02d%02d %02d:%02d:%02d", 
				stCurTM->tm_year+2000, stCurTM->tm_mon+1, stCurTM->tm_mday,
				stCurTM->tm_hour, stCurTM->tm_min, stCurTM->tm_sec);
	}

	return 0;
}

//---------------------------------------------
int TimeUtil_GetExactCurDateTime(char* szCurDateTime, uint32_t iSize)
{
	if(szCurDateTime == NULL)
	{
		return -1;
	}

	//
	struct timeval tv;
	gettimeofday(&tv, NULL);

	char szDateTime[80];
	TimeUtil_GetCurDateTime(szDateTime, sizeof(szDateTime));

	snprintf(szCurDateTime, iSize, "%s.%.6lu", szDateTime, tv.tv_usec);

	return 0;
}

//---------------------------------------------
int TimeUtil_GetExactCurDateTime2(char* szCurDateTime, uint32_t iSize)
{
	if(szCurDateTime == NULL)
	{
		return -1;
	}

	//
	struct timeval tv;
	gettimeofday(&tv, NULL);

	char szDateTime[80];
	TimeUtil_GetCurDateTime2(szDateTime, sizeof(szDateTime));

	snprintf(szCurDateTime, iSize, "%s.%.6lu", szDateTime, tv.tv_usec);

	return 0;
}



//---------------------------------------------
int TimeUtil_GetTime(char* szCurDateTime, uint32_t iSize)
{
	if(szCurDateTime == NULL)
	{
		return -1;
	}
		
	time_t stCurTime = time(NULL);
	struct tm * stCurTM = localtime(&stCurTime);

	snprintf(szCurDateTime, iSize, "%02d:%02d:%02d",
			stCurTM->tm_hour, stCurTM->tm_min, stCurTM->tm_sec);
	

	return 0;
}

//---------------------------------------------
int TimeUtil_GetTime2(char* szCurDateTime, uint32_t iSize)
{
	if(szCurDateTime == NULL)
	{
		return -1;
	}

	time_t stCurTime = time(NULL);
	struct tm * stCurTM = localtime(&stCurTime);

	snprintf(szCurDateTime, iSize, "%02d:%02d:%02d",
			stCurTM->tm_hour, stCurTM->tm_min, stCurTM->tm_sec);

	return 0;
}

