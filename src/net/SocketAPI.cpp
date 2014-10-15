#include "SocketAPI.h"

#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * 创建socket.
 * iSockType: SOCK_STREAM, SOCK_DGRAM
 */
int	socketapi_socket(int domain, int type, int protocol)
{
	return ::socket(domain, type, protocol);
}

/*
 * 绑定socket.
 */
int socketapi_bind(int sockfd, const struct sockaddr* addr, uint32_t addrlen)
{
	return ::bind(sockfd, addr, addrlen);
}

/*
 * 连接
 */
int socketapi_connect(int sockfd, const struct sockaddr * addr, uint32_t addrlen)
{
	return ::connect(sockfd, addr, addrlen);
}

/*
 * 监听
 */
int socketapi_listen(int sockfd, uint32_t backlog)
{
	return ::listen(sockfd, backlog);
}

/*
 * 接受新连接
 */
SOCKET	socketapi_accept(int sockfd, struct sockaddr* addr, uint32_t* addrlen)
{
	return ::accept(sockfd, addr, addrlen);
}


/*
 * 地址是否可复用
 */
int socketapi_isreuse(int sockfd)
{
	int reuse = 0;
	uint32_t len = sizeof(reuse);

	socketapi_getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, &len);
	
	if(reuse == 1)
	{
		return 1;
	}

	return 0;
}

/*
 * 设置地址复用
 */
int socketapi_setreuse(int sockfd, int on)
{
	if(on != 0 && on != 1)
	{
		return -1;
	}

	int opt = on;
	int iRet = socketapi_setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
									&opt, sizeof(opt));
	if(iRet < 0)
	{
		return -2;
	}

	return 0;
}


/*
 * 根据sockfd获取本地的相应地址.
 */
int	socketapi_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return ::getsockname(sockfd, addr, addrlen);
}

/*
 * 根据sockdf获取对端的相应地址.
 */
int	socketapi_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return ::getpeername(sockfd, addr, addrlen);
}

/*
 * 获取socket选项
 */
int socketapi_getsockopt(int sockfd, int level, int optname, void* optval, uint32_t* optlen)
{
	return ::getsockopt(sockfd, level, optname, optval, optlen);
}

/*
 * 设置socket选项
 */
int socketapi_setsockopt(int sockfd, int level, int optname, const void* optval, uint32_t optlen)
{
	return ::setsockopt(sockfd, level, optname, optval, optlen);
}

/*
 * 发送len个字节的buf.
 */
int	socketapi_send(int sockfd, const void* buf, uint32_t len, uint32_t flags)
{
	return ::send(sockfd, buf, len, flags);
}

/*
 * udp方式， 发送len个字节的buf给对端to
 */
int socketapi_sendto(int sockfd, const void* cBuf, int iLen, uint32_t iFlags, 
						  const struct sockaddr* to, int iToLen)
{
	return ::sendto(sockfd, cBuf, iLen, iFlags, to, iToLen);
	
}

/*
 * 从socket接受len个字节的buf
 */
int	socketapi_recv(int sockfd, void * buf, uint32_t len, uint32_t flags)
{
	return ::recv(sockfd, buf, len, flags);
}

/*
 * 从from端接受len个字节的buf
 */
int	socketapi_recvfrom(int sockfd, void* buf, int len, uint32_t flags, 
							struct sockaddr* from, uint32_t* fromlen)
{
	return ::recvfrom(sockfd, buf, len, flags, from, fromlen);
}

/*
 * 关闭socket.
 */
int socketapi_close(int sockfd)
{
	return ::close(sockfd);
}

/*
 * 判断socket是否非阻塞
 */
int socketapi_getnonblocking(int sockfd)
{
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	return flags | O_NONBLOCK;
}

/*
 * 设置socket为非阻塞
 */
int socketapi_setnonblocking(int sockfd, int on)
{
	int flags = ::fcntl(sockfd, F_GETFL, 0);

	if (on)
	{
		// make nonblocking fd
		flags |= O_NONBLOCK;
	}
	else
	{
		// make blocking fd
		flags &= ~O_NONBLOCK;
	}

	::fcntl(sockfd, F_SETFL, flags);
	return 0;
}

/*
 * 判断socket是否available.
 */
uint32_t socketapi_available(int sockfd)
{
	uint32_t arg = 0;
	::ioctl(sockfd, FIONREAD, &arg);
	return arg;
}


/*
 * 内核缺省close操作是立即返回，如果有数据残留在套接口缓冲区中
 * 则系统将试着将这些数据发送给对方。SO_LINGER选项用来改变此缺省设置
 * 
 * 1. l_onoff=0，则该选项关闭，等于缺省close调用立即返回，传输任何未发送的数据
 *
 * 2. l_onoff非0，l_linger=0，则套接口关闭时TCP夭折连接，TCP将丢弃
 *		保留在套接口发送缓冲区中的任何数据并发送一个RST给对方，而不是通常
 *		的四分组终止序列，这避免了TIME_WAIT状态；
 *
 * 3. l_onoff非0，l_linger非0，当套接口关闭时内核将拖延一段时间
 *		（由l_linger决定）。如果套接口缓冲区中仍残留数据，进程将处于睡眠状态，
 *		直 到（a）所有数据发送完且被对方确认，之后进行正常的终止序列（描述
 *		字访问计数为0）或（b）延迟时间到。
 */
int socketapi_setlinger(int iSocket, int lingertime) 
{
	struct linger ling;

	ling.l_onoff = lingertime > 0 ? 1 : 0;
	ling.l_linger = lingertime;

	int iRet = socketapi_setsockopt(iSocket, SOL_SOCKET, SO_LINGER, 
								&ling, sizeof(ling));
	if(iRet < 0)
	{
		return -1;
	}

	return 0;
}

/*
 * 获取linger选项
 */
int Socket_GetLinger(int iSocket) 
{ 
	struct linger ling;
	uint32_t len = sizeof(ling);

	socketapi_getsockopt(iSocket, SOL_SOCKET, SO_LINGER, &ling, &len);

	return ling.l_linger;
}



/*
 * 关闭socket----	函数允许你只停止在某个方向上的数据传输，
 *					而一个方向上的数据传输继续进行。
 * 
 * SHUT_RD（0）：	关闭sockfd上的读功能，此选项将不允许sockfd进行读操作。
 * SHUT_WR（1）：	关闭sockfd的写功能，此选项将不允许sockfd进行写操作。
 * SHUT_RDWR（2）：	关闭sockfd的读写功能
 */
int socketapi_shutdown(int sockfd, uint32_t how)
{
	return ::shutdown(sockfd, how);
}

/*
 * select 方式.
 */
int socketapi_select(int maxfdp1, 
					fd_set* readset, fd_set * writeset, fd_set * exceptset,
					struct timeval * timeout)
{
	return ::select(maxfdp1, readset, writeset, exceptset, timeout);
}