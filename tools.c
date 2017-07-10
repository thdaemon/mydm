/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is an opensource (free) software
 */

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "su.h"
#include "tools.h"

void err_quit(char* msg)
{
	perror(msg);
	exit(1);
}

ssize_t mydm_print(char* fmt, ...)
{
	int fd;
	va_list ap;
	char buff[1024];
	ssize_t retval;

	fd = STDERR_FILENO;
	retval = 0;

	va_start(ap, fmt);
	vsnprintf(buff, sizeof(buff), fmt, ap);

	if (write(fd, "[mydm] ", 7) != 7) return -1;
	retval = write(fd, buff, strlen(buff));

	va_end(ap);

	return retval;
}

void* my_signal(int signum, void* handler, int restartsyscall)
{
	struct sigaction sa, osa;

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	if (restartsyscall) {
		sa.sa_flags = SA_RESTART;
	} else {
#ifdef SA_INTERRUPT
		sa.sa_flags = SA_INTERRUPT;
#endif
	}
	if(sigaction(signum, &sa, &osa) < 0)
		return (void*) -1;
	return osa.sa_handler;
}

int unblock_signal(int signum)
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, signum);
	return sigprocmask(SIG_UNBLOCK, &set, &oset);
}

int block_signal(int signum)
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, signum);
	return sigprocmask(SIG_BLOCK, &set, &oset);
}

int exec_try_login_user(char* username, char* file, int no_system_su)
{
	if (username) {
		if (no_system_su) {
			if (switch_user(username) < 0) {
				mydm_print("Can not login user '%s'\n", username);
				return -1;
			}
			return execlp(file, file, NULL);
		} else {
			return execlp("su", "su", "-", username, "-c", file, NULL);
		}
	} else {
		return execlp(file, file, NULL);
	}
}
