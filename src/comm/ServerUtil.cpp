#include "ServerUtil.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>

//extern int daemon_proc;

#define MAXFD	65535


/* 
 * daemon进程
 */
int ServerUtil_DaemonInit(const char* pName)
{
	int i;
	pid_t pid;

	if( (pid = fork() ) < 0 )
	{
		return -1;
	}
	else if(pid)
	{	
		exit(0);
	}

	if(setsid() < 0)
	{
		return -1;
	}

	signal(SIGHUP, SIG_IGN);
	if( (pid=fork()) < 0 )
	{
		return -1;
	}
	else if(pid)
	{
		exit(0);
	}

	// daemon_proc = 1;

	chdir("/");

	// 关闭文件描述符
	for(i=0; i<MAXFD; i++)
	{
		close(i);
	}

	// 重定向stdin, stdout, stderr 至 /dev/null
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	openlog(pName, LOG_PID, 0);

	return 0;
}

