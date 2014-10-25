/***********************************************
 * @file	logger.h
 * 
 * @brief	������־��֧��ѭ��д
 * @author	junlewang 	2012.4.22
 ***********************************************/

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "LiteType.h"
#include <stdio.h>

/*
 * д��־�ķ�ʽ
 */
typedef enum {
	T_LOG_LOOP	= 0,			// ���ļ���Сѭ��д
	T_LOG_DAY		= 1,			// ����д
	T_LOG_HOUR	= 2,			// ��Сʱд
} LOG_TYPE;



/*
 * @breif 	��־������
 */
typedef enum {
	L_LOG_OFF		= 1,
	L_LOG_ERROR		= 2,
	L_LOG_WARN		= 3,
	L_LOG_INFO		= 4,
	L_LOG_DEBUG		= 5,
	L_LOG_DEBUGBUF	= 6,
	L_LOG_ALL		= 7
} LOG_LEVEL;

/*
 * @breif 	helper ��
 */
#define XLOG_ERROR(logger,fmt,args...)	\
    do{\
		if( ((logger) != NULL) \
			&& ((logger)->iLogLevel >= L_LOG_ERROR) ){\
			Logger_Log(logger, "[ERROR] "fmt" [%s:%d:%s()]",##args,__FILE__,__LINE__,__FUNCTION__);\
	    }\
	}while(0)

#define XLOG_WARN(logger,fmt,args...)	\
    do{\
	    if( ((logger) != NULL) \
		&& ((logger)->iLogLevel >= L_LOG_WARN) ){\
			Logger_Log(logger, "[WARN] "fmt,##args);\
	    }\
	}while(0)

#define XLOG_INFO(logger,fmt,args...)	\
    do{\
		if( ((logger) != NULL) \
		&& ((logger)->iLogLevel >= L_LOG_INFO) ){\
			Logger_Log(logger, "[INFO] "fmt,##args);\
		}\
	}while(0)

#define XLOG_DEBUG(logger,fmt,args...)	\
    do{\
		if( ((logger) != NULL) \
		&& ((logger)->iLogLevel >= L_LOG_DEBUG) ){\
		    Logger_Log(logger, "[DEBUG] "fmt" [%s:%d:%s()]",##args,__FILE__,__LINE__,__FUNCTION__);\
		}\
	}while(0)




//#define LOG_INIT(filename,level,)		do{Logger::GetInstance()->Init()}while(0)


typedef struct {
	char			szBaseFileName[256];
	char			szLogFileName[256];
	int				iLogLevel;			// ��־����
	int				iType;				// д��־��ʽ
	uint32_t		dwMaxSize;			// ÿ����־��󳤶�
	time_t			stLastTime;

	uint32_t		dwMaxNum;				// ��־����
	uint32_t		dwCurNum;				// ��ǰд����־��
} Logger;

int Logger_Log(Logger* plogger, const char* msg, ...);

int Logger_Init(Logger* plogger, const char* szFileBaseName, 
				int iLogLevel, int iType, 
				uint32_t dwMaxSize, uint32_t cLogNum);


#endif
