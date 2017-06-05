#define _POSIX_C_SOURCE 200819L

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
			if (switch_user(username) < 0)
				return -1;
			return execlp(file, file, 0);
		} else {
			return execlp("su", "su", "-", username, "-c", file, 0);
		}
	} else {
		return execlp(file, file, 0);
	}
}
