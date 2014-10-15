/**********************************************************************
 * SocketInputStream.h
 * 
 * @brief		消息数据的接收流
 * @author		jolly
 * @date		2012.10.1
 **********************************************************************/

#ifndef _LITE_SOCKETOUTPUTSTREAM_H_
#define _LITE_SOCKETOUTPUTSTREAM_H_

#include "comm/LiteType.h"

//初始化的接收缓存长度
#define MAX_OUTPUTSTREAM_SIZE				(10*4096)

typedef struct  {
	int			iSocket;
	uint32_t	dwHead;
	uint32_t	dwTail;
	char		cBuf[MAX_OUTPUTSTREAM_SIZE];
} SocketOutputStream;

// 初始化
int			SocketOutputStream_Init(SocketOutputStream* pStream, int iSocket);
	
// 从缓存中读取len个字节，移动指针
int			SocketOutputStream_Read(SocketOutputStream* pStream, char* cBuf, uint32_t dwLen);

// 向缓存区写len个字节
int			SocketOutputStream_Write(SocketOutputStream* pStream, const char* buf, uint32_t* pdwLen);

// 从缓存中读取len个字节，不移动指针
int			SocketOutputStream_Peek(SocketOutputStream* pStream, char* cBuf, uint32_t dwLen);

// 寻找封包符
//int		Find(char* cBuf, uint32_t dwSepSize);

// 跳过len个字节
int			SocketOutputStream_Skip(SocketOutputStream* pStream, uint32_t dwLen);

// OutputStream
int			SocketOutputStream_Flush(SocketOutputStream* pStream);

// InputStream
int			SocketOutputStream_Fill(SocketOutputStream* pStream);

// 缓冲区数据量
uint32_t	SocketOutputStream_Capacity(SocketOutputStream* pStream);
uint32_t	SocketOutputStream_Length(SocketOutputStream* pStream);
int			SocketOutputStream_IsEmpty(SocketOutputStream* pStream);

// 清空缓冲区
void		SocketOutputStream_CleanUp();

#endif
