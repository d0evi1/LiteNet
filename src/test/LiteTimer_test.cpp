#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <unistd.h>

#include "comm/LiteTimer.h"

void ExpireHello(uint32_t dwDataLen, char* pcData)
{
	printf("hello world\n");
}

/*
 * 
 */
int main()
{
	LiteTimer stTimer;

	stTimer.InitTimer(100);
	
	char data[32] = "hello world";
	//char* pData = data;
	TimerNode stNode;
	bzero(&stNode, sizeof(TimerNode));
	stNode.OnExpire = ExpireHello;
	stNode.dwDataLen = sizeof(data);
	memcpy(stNode.pcData, data, sizeof(data));
	
	printf("func address: 0x%x\n", stNode.OnExpire);
	printf("curNode address:0x%x\n", &stNode);

	// 200us µ½ÆÚ.
	stTimer.AddTimer(200, &stNode);
	for(int i=0; i<2; i++)
	{
		printf("===================================\n");
		stTimer.CheckTimer();
		sleep(1);
	}

	return 0;
}