/*****************************************************************
 * 对原始套接字的一层封装 
 * 
 * @brief		xxx
 * @author		jolly
 * @date		2012.
 ******************************************************************/

#ifndef _LITE_SOCKETAPI_H_
#define _LITE_SOCKETAPI_H_

#include "comm/LiteType.h"
#include <netinet/in.h>

typedef int SOCKET;

#define INVALID_SOCKET				-1
#define	SOCKET_ERROR				-1
#define	SOCKET_ERROR_WOULDBLOCK		-100


// 正常通信函数
int		socketapi_socket(int domain, int type, int protocol);
int		socketapi_bind(int sockfd, const struct sockaddr* addr, uint32_t addrlen);
int		socketapi_connect(int sockfd, const struct sockaddr* addr, uint32_t addrlen);
int		socketapi_listen(int sockfd, uint32_t backlog);
int		socketapi_accept(int sockfd, struct sockaddr* addr, uint32_t* addrlen);


// 复用地址
int		socketapi_isreuse(int sockfd);
int		socketapi_setreuse(int sockfd, int on);

// 获取addr
int		socketapi_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int		socketapi_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

// 获取socket选项
int		socketapi_getsockopt(int sockfd, int level, int optname, 
							 void* optval, uint32_t* optlen);
int		socketapi_setsockopt(int sockfd, int level, int optname, 
							const void* optval, uint32_t optlen);

// 发送
int		socketapi_send(int sockfd, const void* buf, uint32_t len, uint32_t flags);
int		socketapi_sendto(int sockfd, const void* buf, int len, uint32_t flags, 
						const struct sockaddr* to , int tolen);

// 接收
int		socketapi_recv(int sockfd, void * buf, uint32_t len, uint32_t flags=0);
int		socketapi_recvfrom(int sockfd, void* buf, int len, uint32_t flags, 
						struct sockaddr* from, uint32_t* fromlen);

// 关闭socket
int		socketapi_shutdown(int sockfd, uint32_t how);
int		socketapi_close(int sockfd);

// 非阻塞
int		socketapi_getnonblocking(int sockfd);
int		socketapi_setnonblocking(int sockfd, int on);

// linger选项
int		socketapi_setlinger(int sockfd, int lingertime);
int		socketapi_getlinger(int sockfd);

// 接收缓冲区
int		socketapi_getrecvbufsize(int sockfd, uint32_t* pdwSize);
int		socketapi_setrecvbufsize(int sockfd, uint32_t size);

// 发送缓冲区
int		socketapi_getsendbufsize(int sockfd, uint32_t* pdwSize);
int		socketapi_setsendBufSize(int sockfd, uint32_t size);

// 判断socket是否出错
int		socketapi_iserr(int sockfd);

//
uint32_t	socketapi_available(int sockfd);



int	socketapi_select(int maxfdp1, fd_set* readset, fd_set* writeset, fd_set* exceptset, 
						struct timeval* timeout);

#endif