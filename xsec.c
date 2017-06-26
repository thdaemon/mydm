/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is an opensource (free) software
 */

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>
#include <pwd.h>

#include "tools.h"

extern int killxsvr();

#ifdef CONFIG_ENABLE_XSEC
static int setenv_XAUTHORITY(const char *xauth_file, const char* home)
{
	if (xauth_file)
		return setenv("XAUTHORITY", xauth_file, 1);

	char filename[PATH_MAX + 1];

	snprintf(filename, PATH_MAX, "%s/.Xauthority", home);
	return setenv("XAUTHORITY", filename, 1);
}

static struct passwd *getpwnamuid(const char *username)
{
	if (username) {
		return getpwnam(username);
	} else {
		return getpwuid(getuid());
	}
}

int xauth_magic_cookie_prepare_filename(const char *username, const char *xauth_file)
{
	struct passwd *pwd;

	if ((pwd = getpwnamuid(username)) == NULL)
		return -1;

	return setenv_XAUTHORITY(xauth_file, pwd->pw_dir);
}

int xauth_magic_cookie_gen(const char *display, const char *username)
{
	pid_t pid;
	int status;
	struct passwd *pwd;
	char *filename;

	if ((pwd = getpwnamuid(username)) == NULL)
		return -1;

	if ((filename = getenv("XAUTHORITY")) == NULL)
		return -1;

	block_signal(SIGCHLD);
	if ((pid = fork()) < 0)
		err_quit("fork");

	if (pid == 0) {
		unblock_signal(SIGCHLD);

		my_signal(SIGTTIN, SIG_IGN, 1);
		my_signal(SIGTTOU, SIG_IGN, 1);

		execlp("xauth", "xauth", "generate", display, "MIT-MAGIC-COOKIE-1", NULL);
		fprintf(stderr, "exec xauth error: %s\n", strerror(errno));

		return -1;
	}

	while (waitpid(pid, &status, 0) != pid) {
		if (errno != EINTR) {
			return -1;
		}
	}

	unblock_signal(SIGCHLD);

	if ((!WIFEXITED(status)) || (WEXITSTATUS(status) != 0)) {
		fprintf(stderr, "xauth_magic_cookie_gen: xauth exited failed.\n");
		return -1;
	}

	if (chmod(filename, S_IRUSR|S_IWUSR) < 0)
		return -1;

	if (chown(filename, pwd->pw_uid, pwd->pw_gid) < 0)
		return -1;

	return 0;
}
#else
void uncompiled_function(const char* name, int kill_server)
{
	fprintf(stderr, "%s: this feature is not compiled.\n", name);
	if (kill_server)
		killxsvr();
	exit(1);
}

int xauth_magic_cookie_prepare_filename(const char *username, const char *xauth_file)
{
	uncompiled_function("xauth_magic_cookie_prepare_filename", 0);
	return 0;
}

int xauth_magic_cookie_gen(const char *display, const char *username)
{
	uncompiled_function("xauth_magic_cookie_gen", 1);
	return 0;
}
#endif /* CONFIG_ENABLE_XSEC */
