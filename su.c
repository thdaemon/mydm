/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is a opensource (free) software
 */

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

extern char **environ;

#define setenv_er(name, value) do { \
                                      if (setenv((name), (value), 1) < 0) \
                                            return -1; \
                               } while (0);

char* save_evs[] = { "PATH", "TERM", "LANG", "LANGUAGE", "LC_CTYPE", "LC_NUMERIC",
                     "LC_TIME", "LC_COLLATE", "LC_MONETARY", "LC_MESSAGES", "LC_PAPER", 
                     "LC_NAME", "LC_ADDRESS", "LC_TELEPHONE", "LC_MEASUREMENT", 
                     "LC_IDENTIFICATION" };

int switch_user(char* username)
{
	struct passwd* pwd;
	char** environ_save;

	if ((pwd = getpwnam(username)) == NULL)
		return -1;

	if (initgroups(pwd->pw_name, pwd->pw_gid) < 0)
		return -1;

	if ((setgid(pwd->pw_gid) < 0) || (setuid(pwd->pw_uid) < 0))
		return -1;

	chdir(pwd->pw_dir);

	environ_save = environ;
	environ = NULL;

	setenv_er("HOME", pwd->pw_dir);
	setenv_er("USER", pwd->pw_name);
	setenv_er("LOGNAME", pwd->pw_name);
	setenv_er("SHELL", pwd->pw_shell);

	int i = 0;
	while (environ_save[i]) {
		for (int j = 0; j < sizeof(save_evs) / sizeof(char*); j++) {
			if (memcmp(environ_save[i], save_evs[j], strlen(save_evs[j])) == 0) {
				putenv(environ_save[i]);
			}
		}
		i++;
	}

	return 0;
}
