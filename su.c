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
                                            goto cleanup_save_env_err; \
                               } while (0);

static char* save_evs[] = { "DISPLAY", "PATH", "TERM", "LANG", "LANGUAGE", "LC_CTYPE",
                            "LC_NUMERIC", "LC_TIME", "LC_COLLATE", "LC_MONETARY",
                            "LC_MESSAGES", "LC_PAPER", "LC_NAME", "LC_ADDRESS",
                            "LC_TELEPHONE", "LC_MEASUREMENT", "LC_IDENTIFICATION",
                            "LC_ALL" };

static char* save_evs_values[sizeof(save_evs) / sizeof(char*)];

static int save_env(char* value, int n)
{
	char* buffer;
	size_t length;

	length = strlen(value);

	if ((buffer = malloc(length + 1)) == NULL) {
		errno = ENOMEM;
		return -1;
	}

	strcpy(buffer, value);

	save_evs_values[n] = buffer;
	return 0;
}

int switch_user(char* username)
{
	struct passwd* pwd;

	memset(save_evs_values, 0, sizeof(save_evs_values));

	if ((pwd = getpwnam(username)) == NULL)
		return -1;

	if (initgroups(pwd->pw_name, pwd->pw_gid) < 0)
		return -1;

	if ((setgid(pwd->pw_gid) < 0) || (setuid(pwd->pw_uid) < 0))
		return -1;

	if (chdir(pwd->pw_dir) < 0)
		return -1;

	int i = 0;
	while (environ[i]) {
		for (int j = 0; j < (sizeof(save_evs) / sizeof(char*)); j++) {
			size_t length = strlen(save_evs[j]);

			if ((memcmp(environ[i], save_evs[j], length) == 0)
			    && (environ[i][length] == '=')) {
				if (save_env(environ[i], j) < 0)
					goto cleanup_save_env_err;
			}
		}
		i++;
	}

	environ = NULL;
	setenv_er("HOME", pwd->pw_dir);
	setenv_er("USER", pwd->pw_name);
	setenv_er("LOGNAME", pwd->pw_name);
	setenv_er("SHELL", pwd->pw_shell);

	for (int n = 0; n < (sizeof(save_evs) / sizeof(char*)); n++) {
		if (save_evs_values[n]) {
			char* value = strchr(save_evs_values[n], '=');

			if (!value || *value == 0) {
				free(save_evs_values[n]);
				continue;
			}

			value++;
			setenv_er(save_evs[n], value);

			free(save_evs_values[n]);
			save_evs_values[n] = NULL;
		}
	}

	return 0;

cleanup_save_env_err:
	for (int n = 0; n < (sizeof(save_evs) / sizeof(char*)); n++) {
		if (save_evs_values[n])
			free(save_evs_values[n]);
	}

	return -1;
}
