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
 * ����socket.
 * iSockType: SOCK_STREAM, SOCK_DGRAM
 */
int	socketapi_socket(int domain, int type, int protocol)
{
	return ::socket(domain, type, protocol);
}

/*
 * ��socket.
 */
int socketapi_bind(int sockfd, const struct sockaddr* addr, uint32_t addrlen)
{
	return ::bind(sockfd, addr, addrlen);
}

/*
 * ����
 */
int socketapi_connect(int sockfd, const struct sockaddr * addr, uint32_t addrlen)
{
	return ::connect(sockfd, addr, addrlen);
}

/*
 * ����
 */
int socketapi_listen(int sockfd, uint32_t backlog)
{
	return ::listen(sockfd, backlog);
}

/*
 * ����������
 */
SOCKET	socketapi_accept(int sockfd, struct sockaddr* addr, uint32_t* addrlen)
{
	return ::accept(sockfd, addr, addrlen);
}


/*
 * ��ַ�Ƿ�ɸ���
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
 * ���õ�ַ����
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
 * ����sockfd��ȡ���ص���Ӧ��ַ.
 */
int	socketapi_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return ::getsockname(sockfd, addr, addrlen);
}

/*
 * ����sockdf��ȡ�Զ˵���Ӧ��ַ.
 */
int	socketapi_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return ::getpeername(sockfd, addr, addrlen);
}

/*
 * ��ȡsocketѡ��
 */
int socketapi_getsockopt(int sockfd, int level, int optname, void* optval, uint32_t* optlen)
{
	return ::getsockopt(sockfd, level, optname, optval, optlen);
}

/*
 * ����socketѡ��
 */
int socketapi_setsockopt(int sockfd, int level, int optname, const void* optval, uint32_t optlen)
{
	return ::setsockopt(sockfd, level, optname, optval, optlen);
}

/*
 * ����len���ֽڵ�buf.
 */
int	socketapi_send(int sockfd, const void* buf, uint32_t len, uint32_t flags)
{
	return ::send(sockfd, buf, len, flags);
}

/*
 * udp��ʽ�� ����len���ֽڵ�buf���Զ�to
 */
int socketapi_sendto(int sockfd, const void* cBuf, int iLen, uint32_t iFlags, 
						  const struct sockaddr* to, int iToLen)
{
	return ::sendto(sockfd, cBuf, iLen, iFlags, to, iToLen);
	
}

/*
 * ��socket����len���ֽڵ�buf
 */
int	socketapi_recv(int sockfd, void * buf, uint32_t len, uint32_t flags)
{
	return ::recv(sockfd, buf, len, flags);
}

/*
 * ��from�˽���len���ֽڵ�buf
 */
int	socketapi_recvfrom(int sockfd, void* buf, int len, uint32_t flags, 
							struct sockaddr* from, uint32_t* fromlen)
{
	return ::recvfrom(sockfd, buf, len, flags, from, fromlen);
}

/*
 * �ر�socket.
 */
int socketapi_close(int sockfd)
{
	return ::close(sockfd);
}

/*
 * �ж�socket�Ƿ������
 */
int socketapi_getnonblocking(int sockfd)
{
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	return flags | O_NONBLOCK;
}

/*
 * ����socketΪ������
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
 * �ж�socket�Ƿ�available.
 */
uint32_t socketapi_available(int sockfd)
{
	uint32_t arg = 0;
	::ioctl(sockfd, FIONREAD, &arg);
	return arg;
}


/*
 * �ں�ȱʡclose�������������أ���������ݲ������׽ӿڻ�������
 * ��ϵͳ�����Ž���Щ���ݷ��͸��Է���SO_LINGERѡ�������ı��ȱʡ����
 * 
 * 1. l_onoff=0�����ѡ��رգ�����ȱʡclose�����������أ������κ�δ���͵�����
 *
 * 2. l_onoff��0��l_linger=0�����׽ӿڹر�ʱTCPز�����ӣ�TCP������
 *		�������׽ӿڷ��ͻ������е��κ����ݲ�����һ��RST���Է���������ͨ��
 *		���ķ�����ֹ���У��������TIME_WAIT״̬��
 *
 * 3. l_onoff��0��l_linger��0�����׽ӿڹر�ʱ�ں˽�����һ��ʱ��
 *		����l_linger������������׽ӿڻ��������Բ������ݣ����̽�����˯��״̬��
 *		ֱ ����a���������ݷ������ұ��Է�ȷ�ϣ�֮�������������ֹ���У�����
 *		�ַ��ʼ���Ϊ0����b���ӳ�ʱ�䵽��
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
 * ��ȡlingerѡ��
 */
int Socket_GetLinger(int iSocket) 
{ 
	struct linger ling;
	uint32_t len = sizeof(ling);

	socketapi_getsockopt(iSocket, SOL_SOCKET, SO_LINGER, &ling, &len);

	return ling.l_linger;
}



/*
 * �ر�socket----	����������ֹֻͣ��ĳ�������ϵ����ݴ��䣬
 *					��һ�������ϵ����ݴ���������С�
 * 
 * SHUT_RD��0����	�ر�sockfd�ϵĶ����ܣ���ѡ�������sockfd���ж�������
 * SHUT_WR��1����	�ر�sockfd��д���ܣ���ѡ�������sockfd����д������
 * SHUT_RDWR��2����	�ر�sockfd�Ķ�д����
 */
int socketapi_shutdown(int sockfd, uint32_t how)
{
	return ::shutdown(sockfd, how);
}

/*
 * select ��ʽ.
 */
int socketapi_select(int maxfdp1, 
					fd_set* readset, fd_set * writeset, fd_set * exceptset,
					struct timeval * timeout)
{
	return ::select(maxfdp1, readset, writeset, exceptset, timeout);
}