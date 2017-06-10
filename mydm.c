/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is a opensource (free) software
 *
 * A Simple Display Manager for Linux, it will start a Xorg Server and log a user to run X Client
 */

#define _POSIX_C_SOURCE 200819L

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#ifdef CONFIG_OWN_LIBX11DEV
#define Display void
extern Display* XOpenDisplay(char*);
extern int XCloseDisplay(Display*);
#else
#include <X11/Xlib.h>
#endif /* CONFIG_OWN_LIBX11DEV */

#include "tools.h"

#define DEFAULT_XCLIENT "startxfce4"

int xstart = 0;
pid_t xsvrpid = 0, xclipid = 0;

int killxsvr()
{
	fprintf(stderr, "killing X server\n");
	return kill(xsvrpid, SIGTERM);
}

void sig_user1(int signo)
{
	fprintf(stderr, "X server should be in running\n");
	xstart = 1;
}

void sig_child(int signo)
{
	int status;
	pid_t pid;

	pid = wait(&status);

	if (waitpid(pid, NULL, WNOHANG) >= 0)
		return;

	if (pid == xsvrpid) {
		fprintf(stderr, "X server exited\n");
		exit(0);
	}

	if (pid == xclipid) {
		fprintf(stderr, "X client exited\n");
		killxsvr();
		exit(0);
	}
}

void sig_term(int signo)
{
	fprintf(stderr, "catch SIGTERM/SIGINT, exiting\n");
	if (xclipid)
		kill(xclipid, SIGTERM);
	//killxsvr();
}

int main(int argc, char* argv[])
{
	int opt, no_system_su = 0;
	pid_t pid;
	Display* display;
	char *arg_display, *arg_vt, *arg_xclient, *arg_run, *arg_xserver, *arg_user;

	arg_display = ":0";
	arg_vt = "vt7";
	arg_xclient = DEFAULT_XCLIENT;
	arg_run = NULL;
	arg_xserver = "X";
	arg_user = NULL;

	while ((opt = getopt(argc, argv, "d:v:c:r:s:u:l:nh")) != -1) {
		switch (opt) {
		case 'd':
			arg_display = (char*)optarg;
			break;
		case 'v':
			arg_vt = (char*)optarg;
			break;
		case 'c':
			arg_xclient = (char*)optarg;
			break;
		case 'r':
			arg_run = (char*)optarg;
			break;
		case 's':
			arg_xserver = (char*)optarg;
			break;
		case 'u':
		case 'l':
			arg_user = (char*)optarg;
			break;
		case 'n':
			no_system_su = 1;
			break;
		case 'h':
		case '?':
			printf("Usage: %s [-d display] [-v vt] [-c program] [-r program] [-s program] [-u username] [-l login] [-n] [-h]\n"
			 " OPTIONS \n"
			 "	-d display         Display name, default ':0' \n"
			 "	-v vt              VT number, default 'vt7'\n"
			 "	-c program         X client, default '" DEFAULT_XCLIENT "'\n"
			 "	-r program         After run x client, will run it, but do not wait it to exit, default null\n"
			 "	-s program         The path of X Server, default 'X'\n"
			 "	-u username        The user to login, default null (Not login)\n"
			 "	-l login           Same as -u\n"
			 "	-n                 Do not use the su command of system (default used)\n"
			 "	-h                 Show this usage\n"
			 "\n EXAMPLES\n"
			 "	%s -d :0 -c gnome-session -u my_user_name\n"
			 "	%s -d :2 -v vt3 -c xterm -r 'exec metacity'\n"
			 "	%s -c '/path/to/myde_init.sh' -r 'exec /path/to/myde_autostart.sh' -u my_user_name\n"
			 , argv[0], argv[0], argv[0], argv[0]);
			exit(0);
			break;
		}
	}

	my_signal(SIGQUIT, SIG_IGN, 1);
	my_signal(SIGTSTP, SIG_IGN, 1);

	my_signal_cld_reset(SIGHUP, SIG_IGN, 1);
	my_signal_cld_reset(SIGTERM, sig_term, 1);
	my_signal_cld_reset(SIGCHLD, sig_child, 1);
	my_signal_cld_reset(SIGUSR1, sig_user1, 1);

	my_signal_cld_ign(SIGINT, sig_term, 1);

	block_signal(SIGCHLD);
	if ((pid = fork()) < 0)
		err_quit("fork");

	if (pid == 0) {
		unblock_signal(SIGCHLD);

		my_signal_cld_ign(SIGINT, SIG_IGN, 1);

		my_signal(SIGTTIN, SIG_IGN, 1);
		my_signal(SIGTTOU, SIG_IGN, 1);

		my_signal(SIGUSR1, SIG_IGN, 1);

		execlp(arg_xserver, arg_xserver, arg_display, arg_vt, "-nolisten", "tcp", 0);
		fprintf(stderr, "exec X server error: %s\n", strerror(errno));

		exit(1);
	}

	xsvrpid = pid;
	unblock_signal(SIGCHLD);

	/* Wait for X Server wake up me. */
	while (!xstart)
		pause();

	my_signal_cld_reset(SIGUSR1, SIG_IGN, 1);

	/* Can the display connect? */
	if ((display = XOpenDisplay(arg_display)) == NULL) {
		fprintf(stderr, "Cannot open display\n");
		killxsvr();
		exit(1);
	}

	XCloseDisplay(display);

	setenv("DISPLAY", arg_display, 1);

	block_signal(SIGCHLD);
	if ((pid = fork()) < 0) {
		killxsvr();
		err_quit("fork");
	}

	if (pid == 0) {
		pid_t cpid = 0;

		unblock_signal(SIGCHLD);
		my_signal_cld_ign(SIGINT, SIG_IGN, 1);

		my_signal_cld_reset(SIGHUP, SIG_DFL, 1);
		my_signal_cld_reset(SIGTERM, SIG_DFL, 1);
		my_signal_cld_reset(SIGCHLD, SIG_DFL, 1);
		my_signal_cld_reset(SIGUSR1, SIG_DFL, 1);

		setpgid(getpid(), getpid());

		if (arg_run != NULL) {
			if ((cpid = fork()) < 0) {
				err_quit("fork");
			}

			if (cpid == 0) {
				exec_try_login_user(arg_user, arg_run, no_system_su);
				fprintf(stderr, "exec %s error: %s\n", arg_run, strerror(errno));
				kill(getppid(), SIGTERM);
				exit(1);
			}
		}
/*
		size_t length = strlen(arg_xclient) + 6;
		char* command = malloc(length);
		if (command == NULL) {
			fprintf(stderr, "malloc: %s", strerror(ENOMEM));
			killxsvr();
			exit(1);
		}
		snprintf(command, length, "exec %s", arg_xclient);
*/
		exec_try_login_user(arg_user, arg_xclient, no_system_su);
		if (cpid)
			kill(cpid, SIGTERM);
		fprintf(stderr, "exec X client '%s' error: %s\n", arg_xclient, strerror(errno));
		exit(1);
	}

	xclipid = pid;
	unblock_signal(SIGCHLD);

	while (1)
		pause();

	exit(0);
}
