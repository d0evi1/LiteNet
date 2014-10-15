#include "ShmUtil.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

/*
 * 返回相应内存块
 */
char* ShmUtil_GetShm(int iKey, int iSize, int iFlag)
{
	int iShmID;
	char* sShm;
	char sErrMsg[50];

	if ((iShmID = shmget(iKey, iSize, iFlag)) < 0) 
	{
		//sprintf(sErrMsg, "shmget %d %d", iKey, iSize);
		//perror(sErrMsg);
		return NULL;
	}
	
	if ((sShm = (char*)shmat(iShmID, NULL,0)) == (char *) -1) 
	{
		//perror("shmat error.");
		return NULL;
	}
	return sShm;
}

/*
 * 初始化
 * @return pstShm
 */
int ShmUtil_GetShm2(void **pstShm, int iShmID, int iSize, int iFlag)
{
	char* sShm = NULL;

	if (!(sShm = ShmUtil_GetShm(iShmID, iSize, iFlag & (~IPC_CREAT)))) 
	{
		if (!(iFlag & IPC_CREAT)) 
		{
			return -1;
		}

		if (!(sShm = ShmUtil_GetShm(iShmID, iSize, iFlag)))
		{
			return -2;
		}
		memset(sShm, 0, iSize);
	}

	*pstShm = sShm;
	return 0;
}

/*
 * 不初始化内存块
 */
int ShmUtil_GetShm3(void **pstShm, int iShmID, int iSize, int iFlag)
{
	char* sShm = NULL;

	if ((sShm = ShmUtil_GetShm(iShmID, iSize, iFlag & (~IPC_CREAT)))==NULL) 
	{
		if (!(iFlag & IPC_CREAT)) 
		{
			return -1;
		}

		if (!(sShm = ShmUtil_GetShm(iShmID, iSize, iFlag))) 
		{
			return -2;
		}

		*pstShm = sShm;
		return 1;
	}

	*pstShm = sShm;
	return 0;
}