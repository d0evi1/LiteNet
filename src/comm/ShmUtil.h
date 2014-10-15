/************************************************************************
 * 共享内存操作工具类
 * 
 * @author  d0evi1	
 * @date	2012.8.15
 ************************************************************************/
#ifndef _LITE_SHMUTIL_H_
#define _LITE_SHMUTIL_H_

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>


/*
 * 创建共享内存
 * @param	iKey	
 * @param	iSize	
 * @param	iFlag	IPC_CREAT ...
 */
char* ShmUtil_GetShm(int iKey, int iSize, int iFlag);

/*
 * 创建共享内存, 初始化内存块
 * @param	pstShm	[inout]
 * @param	iKey	
 * @param	iSize	
 * @param	iFlag	IPC_CREAT ...
 */
int ShmUtil_GetShm2(void **pstShm, int iShmID, int iSize, int iFlag);

/*
 * 创建共享内存， 不初始化内存块
 * @param	pstShm	[inout]
 * @param	iKey	
 * @param	iSize	
 * @param	iFlag	IPC_CREAT ...
 */
int ShmUtil_GetShm3(void **pstShm, int iShmID, int iSize, int iFlag);


#endif
