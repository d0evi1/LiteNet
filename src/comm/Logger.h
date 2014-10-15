/***********************************************
 * @file	logger.h
 * 
 * @brief	单个日志，支持循环写
 * @author	junlewang 	2012.4.22
 ***********************************************/

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "LiteType.h"
#include <stdio.h>

/*
 * 写日志的方式
 */
typedef enum {
	LOG_LOOP	= 0,			// 按文件大小循环写
	LOG_DAY		= 1,			// 按天写
	LOG_HOUR	= 2,			// 按小时写
} LOG_TYPE;



/*
 * @breif 	日志级别定义
 */
typedef enum {
	LOG_OFF			= 1,
	LOG_ERROR		= 2,
	LOG_WARN		= 3,
	LOG_INFO		= 4,
	LOG_DEBUG		= 5,
	LOG_DEBUGBUF	= 6,
	LOG_ALL			= 7
} LOG_LEVEL;

/*
 * @breif 	helper 宏
 */
#define LOG_ERROR(logger,fmt,args...)	do{\
											if( ((logger) != NULL) \
												&& ((logger)->iLogLevel >= LOG_ERROR) ){\
												Logger_Log(logger, "[ERROR] "fmt" [%s:%d:%s()]",##args,__FILE__,__LINE__,__FUNCTION__);\
											}\
										}while(0)

#define LOG_WARN(logger,fmt,args...)	do{\
											if( ((logger) != NULL) \
												&& ((logger)->iLogLevel >= LOG_WARN) ){\
												Logger_Log(logger, "[WARN] "fmt,##args);\
											}\
										}while(0)

#define LOG_INFO(logger,fmt,args...)	do{\
											if( ((logger) != NULL) \
												&& ((logger)->iLogLevel >= LOG_INFO) ){\
												Logger_Log(logger, "[INFO] "fmt,##args);\
											}\
										}while(0)

#define LOG_DEBUG(logger,fmt,args...)	do{\
											if( ((logger) != NULL) \
												&& ((logger)->iLogLevel >= LOG_DEBUG) ){\
												Logger_Log(logger, "[DEBUG] "fmt" [%s:%d:%s()]",##args,__FILE__,__LINE__,__FUNCTION__);\
											}\
										}while(0)




//#define LOG_INIT(filename,level,)		do{Logger::GetInstance()->Init()}while(0)


typedef struct {
	char			szBaseFileName[256];
	char			szLogFileName[256];
	int				iLogLevel;			// 日志级别
	int				iType;				// 写日志方式
	uint32_t		dwMaxSize;			// 每个日志最大长度
	time_t			stLastTime;

	uint32_t		dwMaxNum;				// 日志个数
	uint32_t		dwCurNum;				// 当前写的日志号
} Logger;

int Logger_Log(Logger* plogger, const char* msg, ...);

int Logger_Init(Logger* plogger, const char* szFileBaseName, 
				int iLogLevel, int iType, 
				uint32_t dwMaxSize, uint32_t cLogNum);


#endif
