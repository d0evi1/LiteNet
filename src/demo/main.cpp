
#include "EchoServer.h"

#include <stdio.h>

int main()
{
	// 上下文
	Logger stLogger;
	int iRet = stLogger.Init("/home/jolly/share/workspace/game/LiteTCP/LiteTCP/src/demo/server_log", 
							5, LOG_LOOP, 1000, 5);
	if(iRet < 0)
	{
		printf("Init Log fail: iRet = %d\n", iRet);
		return -1;
	}

	LOG_DEBUG(&stLogger, "server start...");

	EchoServer stServer;
	iRet = stServer.Init(&stLogger, "10.0.2.15", 9995, 1024, 1024, 100);
	if(iRet < 0)
	{
		printf("Init TCPServer fail: iRet = %d\n", iRet);
		return -2;
	}

	// 日志
	stServer.Run();

	printf("server exit...\n");

	return 0;
}