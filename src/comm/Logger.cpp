#include "Logger.h"

#include <memory.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#include "TimeUtil.h"

/*
 * 初始化日志
 */
int Logger_Init(Logger* pLogger, const char* szFileBaseName, 
				int iLogLevel, int iType, 
				uint32_t dwMaxSize, uint32_t dwLogNum)
{
	if(pLogger == NULL || szFileBaseName == NULL)
	{
		return -1;
	}

	memset(pLogger, 0, sizeof(Logger));


	strcpy(pLogger->szBaseFileName, szFileBaseName);
	sprintf(pLogger->szLogFileName, "%s.log", pLogger->szBaseFileName);

	// 1 <= level <= 7 
	if(iLogLevel < 0)
	{
		pLogger->iLogLevel = 1;
	}
	else if(iLogLevel > 7)
	{
		pLogger->iLogLevel = 7;
	}
	
	pLogger->iLogLevel = iLogLevel;
	pLogger->iType = iType;

	pLogger->dwMaxNum = dwLogNum;
	pLogger->dwMaxSize = dwMaxSize;
	pLogger->stLastTime = 0;
	pLogger->dwCurNum = 0;

	return 0;
}


int Logger_ShiftFile(Logger* pLogger)
{
	if(pLogger == NULL)
	{
		return -1;
	}
	
	char szNewName[300];
	struct tm stLogCreateTime;
	struct tm stLogShiftTime;

	// step 1:  判断文件是否存在
	sprintf(pLogger->szLogFileName, "%s.log", pLogger->szBaseFileName);
	struct stat stStat;
	memset(&stStat, 0, sizeof(stStat));
	if(stat(pLogger->szLogFileName, &stStat) < 0) 
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	switch (pLogger->iType) 
	{
	case T_LOG_LOOP:
		if (stStat.st_size < (int)pLogger->dwMaxSize) 
		{
			return 0;
		}
		break;

	case T_LOG_DAY:
		if (pLogger->stLastTime - stStat.st_mtime > 86400) 
		{
			break;
		}

		memcpy(&stLogCreateTime, localtime(&stStat.st_mtime), sizeof(stLogCreateTime));
		memcpy(&stLogShiftTime, localtime(&pLogger->stLastTime), sizeof(stLogShiftTime));
		if (stLogCreateTime.tm_mday == stLogShiftTime.tm_mday) 
		{
			return 0;
		}
		break;

	case T_LOG_HOUR:
		if (pLogger->stLastTime - stStat.st_mtime > 3600) 
		{
			break;
		}
		
		memcpy(&stLogCreateTime, localtime(&stStat.st_mtime), sizeof(stLogCreateTime));
		memcpy(&stLogShiftTime, localtime(&pLogger->stLastTime), sizeof(stLogShiftTime));
		if (stLogCreateTime.tm_hour == stLogShiftTime.tm_hour) 
		{
			return 0;
		}

		break;

	default:
		if (pLogger->dwCurNum < pLogger->dwMaxNum) 
		{
			return 0;
		}

		pLogger->dwCurNum = 0;
		break;
	}

	// step 3: 是否使用新文件
	for(int i = pLogger->dwMaxNum-2; i >= 0; i--)
	{
		if (i == 0)
		{
			sprintf(pLogger->szLogFileName,"%s.log", pLogger->szBaseFileName);
		}
		else
		{
			sprintf(pLogger->szLogFileName,"%s%d.log", pLogger->szBaseFileName, i);
		}

		if (access(pLogger->szLogFileName, F_OK) == 0)
		{
			sprintf(szNewName,"%s%d.log", pLogger->szBaseFileName, i+1);
			if (rename(pLogger->szLogFileName,szNewName) < 0 )
			{
				return -1;
			}
		}
	}
	
	// 更新最后时间
	time(&pLogger->stLastTime);
	return 0;
}


/*
 * 记录日志文件
 */
int Logger_Log(Logger* pLogger, const char* msg, ...)
{
	if(msg == NULL)
	{
		return -1;

	}

	FILE*	pLogFile = fopen(pLogger->szLogFileName, "a+");
	if(pLogFile == NULL)
	{
		return -2;
	}

	char cBuffer[2048];
	memset(cBuffer, 0, 2048);
	va_list stVaList;
	va_start(stVaList, msg);
	vsprintf(cBuffer, msg, stVaList);
	va_end(stVaList);
	
	char szTime[80];
	TimeUtil_GetExactCurDateTime(szTime, sizeof(szTime));
	fprintf(pLogFile, "[%s] %s\n", szTime, cBuffer);

	fclose(pLogFile);
	int iRet = Logger_ShiftFile(pLogger);
	if(iRet < 0)
	{
		return -2;
	}

	return 0;
}
