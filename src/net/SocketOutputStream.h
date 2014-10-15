/**********************************************************************
 * SocketInputStream.h
 * 
 * @brief		��Ϣ���ݵĽ�����
 * @author		jolly
 * @date		2012.10.1
 **********************************************************************/

#ifndef _LITE_SOCKETOUTPUTSTREAM_H_
#define _LITE_SOCKETOUTPUTSTREAM_H_

#include "comm/LiteType.h"

//��ʼ���Ľ��ջ��泤��
#define MAX_OUTPUTSTREAM_SIZE				(10*4096)

typedef struct  {
	int			iSocket;
	uint32_t	dwHead;
	uint32_t	dwTail;
	char		cBuf[MAX_OUTPUTSTREAM_SIZE];
} SocketOutputStream;

// ��ʼ��
int			SocketOutputStream_Init(SocketOutputStream* pStream, int iSocket);
	
// �ӻ����ж�ȡlen���ֽڣ��ƶ�ָ��
int			SocketOutputStream_Read(SocketOutputStream* pStream, char* cBuf, uint32_t dwLen);

// �򻺴���дlen���ֽ�
int			SocketOutputStream_Write(SocketOutputStream* pStream, const char* buf, uint32_t* pdwLen);

// �ӻ����ж�ȡlen���ֽڣ����ƶ�ָ��
int			SocketOutputStream_Peek(SocketOutputStream* pStream, char* cBuf, uint32_t dwLen);

// Ѱ�ҷ����
//int		Find(char* cBuf, uint32_t dwSepSize);

// ����len���ֽ�
int			SocketOutputStream_Skip(SocketOutputStream* pStream, uint32_t dwLen);

// OutputStream
int			SocketOutputStream_Flush(SocketOutputStream* pStream);

// InputStream
int			SocketOutputStream_Fill(SocketOutputStream* pStream);

// ������������
uint32_t	SocketOutputStream_Capacity(SocketOutputStream* pStream);
uint32_t	SocketOutputStream_Length(SocketOutputStream* pStream);
int			SocketOutputStream_IsEmpty(SocketOutputStream* pStream);

// ��ջ�����
void		SocketOutputStream_CleanUp();

#endif
