/************************************************************************
 * �����ڴ����������
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
 * ���������ڴ�
 * @param	iKey	
 * @param	iSize	
 * @param	iFlag	IPC_CREAT ...
 */
char* ShmUtil_GetShm(int iKey, int iSize, int iFlag);

/*
 * ���������ڴ�, ��ʼ���ڴ��
 * @param	pstShm	[inout]
 * @param	iKey	
 * @param	iSize	
 * @param	iFlag	IPC_CREAT ...
 */
int ShmUtil_GetShm2(void **pstShm, int iShmID, int iSize, int iFlag);

/*
 * ���������ڴ棬 ����ʼ���ڴ��
 * @param	pstShm	[inout]
 * @param	iKey	
 * @param	iSize	
 * @param	iFlag	IPC_CREAT ...
 */
int ShmUtil_GetShm3(void **pstShm, int iShmID, int iSize, int iFlag);


#endif
