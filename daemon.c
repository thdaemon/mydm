/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is an opensource (free) software
 */

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <stdlib.h>

#define CTLTERM "/dev/tty"

#include "daemon.h"
#include "tools.h"

static int __daemon(){
	int fd0, fd1, fd2;
	pid_t pid;
	struct sigaction sa;

	umask(0);

	if((pid = fork()) < 0)
		return -1;
	else if(pid > 0)
		exit(0);

	if(setsid() < 0)
		return -1;

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGHUP, &sa, NULL) < 0)
		return -1;

	if((pid = fork()) < 0)
		return -1;
	else if(pid > 0)
		exit(0);

	if(chdir("/") < 0)
		return -1;

	for(int i = 0; i < 1024 ; i++)
		close(i);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(fd0);
	fd2 = dup(fd0);

	if((fd0 != STDIN_FILENO) || (fd1 != STDOUT_FILENO) || (fd2 != STDERR_FILENO)){
		close(fd0);
		close(fd1);
		close(fd2);
		return -1;
	}

	return 0;
}

void do_daemon()
{
	if (__daemon() < 0) {
		mydm_print("Can not initialize daemon\n");
		exit(1);
	}
}
