#include <stdio.h>

#include "comm/Logger.h"


/*
 * »’÷æ≤‚ ‘
 */ 
int main()
{
	Logger stLogger;
	stLogger.Init("/tmp/hello", 5, LOG_LOOP, 1000, 5);

	Logger* pLogger = &stLogger;
	for(int i=0; i<150; i++)
	{
		LOG_ERROR(pLogger, "hello world1");
		LOG_DEBUG(pLogger, "hello world2");
		LOG_WARN(pLogger, "hello world3");
		LOG_INFO(pLogger, "hello world4");
	}

	return 0;
}