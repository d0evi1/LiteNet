/**********************************************************************
 * SocketInputStream.h
 * 
 * @brief		��Ϣ���ݵĽ�����
 * @author		jolly
 * @date		2012.10.1
 **********************************************************************/

#ifndef _LITE_SOCKETINPUTSTREAM_H_
#define _LITE_SOCKETINPUTSTREAM_H_

#include "comm/LiteType.h"

//��ʼ���Ľ��ջ��泤��
#define MAX_INPUTSTREAM_SIZE				(10*4096)

typedef struct  {
	int			iSocket;
	uint32_t	dwHead;
	uint32_t	dwTail;
	char		cBuf[MAX_INPUTSTREAM_SIZE];
} SocketInputStream;

// ��ʼ��
int			SocketInputStream_Init(SocketInputStream* pStream, int iSocket);
	
// �ӻ����ж�ȡlen���ֽڣ��ƶ�ָ��
int			SocketInputStream_Read(SocketInputStream* pStream, void* cBuf, uint32_t dwLen);

// �ӻ����ж�ȡlen���ֽڣ����ƶ�ָ��
int			SocketInputStream_Peek(SocketInputStream* pStream, void* cBuf, uint32_t dwLen);

// ����len���ֽ�
int			SocketInputStream_Skip(SocketInputStream* pStream, uint32_t dwLen);

// InputStream
int			SocketInputStream_Fill(SocketInputStream* pStream);

// ������������
uint32_t	SocketInputStream_Capacity(SocketInputStream* pStream);
uint32_t	SocketInputStream_Length(SocketInputStream* pStream);
int			SocketInputStream_IsEmpty(SocketInputStream* pStream);

// ��ջ�����
void		SocketInputStream_CleanUp();

#endif
