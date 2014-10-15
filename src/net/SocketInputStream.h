/**********************************************************************
 * SocketInputStream.h
 * 
 * @brief		消息数据的接收流
 * @author		jolly
 * @date		2012.10.1
 **********************************************************************/

#ifndef _LITE_SOCKETINPUTSTREAM_H_
#define _LITE_SOCKETINPUTSTREAM_H_

#include "comm/LiteType.h"

//初始化的接收缓存长度
#define MAX_INPUTSTREAM_SIZE				(10*4096)

typedef struct  {
	int			iSocket;
	uint32_t	dwHead;
	uint32_t	dwTail;
	char		cBuf[MAX_INPUTSTREAM_SIZE];
} SocketInputStream;

// 初始化
int			SocketInputStream_Init(SocketInputStream* pStream, int iSocket);
	
// 从缓存中读取len个字节，移动指针
int			SocketInputStream_Read(SocketInputStream* pStream, void* cBuf, uint32_t dwLen);

// 从缓存中读取len个字节，不移动指针
int			SocketInputStream_Peek(SocketInputStream* pStream, void* cBuf, uint32_t dwLen);

// 跳过len个字节
int			SocketInputStream_Skip(SocketInputStream* pStream, uint32_t dwLen);

// InputStream
int			SocketInputStream_Fill(SocketInputStream* pStream);

// 缓冲区数据量
uint32_t	SocketInputStream_Capacity(SocketInputStream* pStream);
uint32_t	SocketInputStream_Length(SocketInputStream* pStream);
int			SocketInputStream_IsEmpty(SocketInputStream* pStream);

// 清空缓冲区
void		SocketInputStream_CleanUp();

#endif
