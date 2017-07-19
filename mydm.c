/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is an opensource (free) software
 *
 * A Simple Display Manager for Linux/Unix, it will start a Xorg Server and log a user to run X Client
 */

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700

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
#include "xsec.h"

#ifndef CONFIG_DEFAULT_XCLIENT
#define CONFIG_DEFAULT_XCLIENT "xterm"
#endif /* CONFIG_DEFAULT_XCLIENT */

/* when repause set to 0, after interrupt, mydm go back to the start of the work. */
int xstart = 0, greeter_mode = 0, repause = 1;
pid_t xsvrpid = 0, xclipid = 0;

void mydmexit(int code)
{
	if (xstart && greeter_mode) {
		repause = 0;
		return;
	}

	exit(code);
}

int killxsvr()
{
	if (xsvrpid) {
		mydm_print("killing X server\n");
		return kill(xsvrpid, SIGTERM);
	}

	exit(1);
}

void sig_user1(int signo)
{
	mydm_print("X server should be in running\n");
	xstart = 1;
}

void sig_child(int signo)
{
	int status;
	pid_t pid;

	/* FIXME: Why should I use WNOHANG???
	 * If I don't, when xauth_magic_cookie_gen() unblock SIGCHLD, 
	 * Linux call handler (It is not fit POSIX), but it will cause 
	 * waitpid/wait block. It should return -1 with ECHILD.
	 */
	pid = waitpid(-1, &status, WNOHANG);

	if ((pid == 0) || (pid == -1))
		return;

	if (waitpid(pid, NULL, WNOHANG) >= 0)
		return;

	if (pid == xsvrpid) {
		mydm_print("X server exited\n");
		mydmexit(0);
	}

	if (pid == xclipid) {
		mydm_print("X client exited\n");
		killxsvr();
	}
}

void sig_term(int signo)
{
	mydm_print("catch SIGTERM/SIGINT, exiting\n");

	/* prevents restart of the session on greeter mode */
	greeter_mode = 0;

	if (xclipid)
		kill(xclipid, SIGTERM);
}

int exec_xserver(char *xserver, char *display, char *vt, int use_xauth, int argc, char *argv[])
{
	int exargs = 0;

	if (use_xauth)
		exargs += 2;

	char *srv_argv[argc + exargs + 6];

	srv_argv[0] = xserver;
	srv_argv[1] = display;
	srv_argv[2] = vt;
	srv_argv[3] = "-nolisten";
	srv_argv[4] = "tcp";

	if (use_xauth) {
		srv_argv[5] = "-auth";
		srv_argv[6] = getenv("XAUTHORITY");
	}

	for (int i = 0; i < argc; i++) {
		srv_argv[i + exargs + 5] = argv[i];
	}

	srv_argv[argc + exargs + 5] = NULL;

	return execvp(xserver, srv_argv);
}

int main(int argc, char *argv[])
{
	int opt, no_system_su = 0, arg_use_xauth = 0;
	pid_t pid;
	Display* display;
	char *arg_display, *arg_vt, *arg_xclient, *arg_run, *arg_xserver, *arg_user;

	arg_display = ":0";
	arg_vt = "vt7";
	arg_xclient = CONFIG_DEFAULT_XCLIENT;
	arg_run = NULL;
	arg_xserver = "X";
	arg_user = NULL;

	while ((opt = getopt(argc, argv, "d:v:c:r:s:u:l:nAgh")) != -1) {
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
		case 'A':
			arg_use_xauth = 1;
			break;
		case 'g':
			greeter_mode = 1;
			break;
		case 'h':
		case '?':
			printf("mydm Display Manager version %s\nCopyright (C) Tian Hao <thxdaemon@gmail.com>\n"
			       "It is an opensource (free) software. This software is "
			       "published under the GNU GPLv3 license.\n\n", PROJECT_VERSION);
			printf("Usage: %s [-d|-v|-c|-r|-s|-u|-l|-n|-A|-g|-h] -- server options\n"
			 " OPTIONS \n"
			 "	-d display         Display name, default ':0' \n"
			 "	-v vt              VT number, default 'vt7'\n"
			 "	-c program         X client, default '" CONFIG_DEFAULT_XCLIENT "'\n"
			 "	-r program         After run x client, will run it, but do not wait it to exit, default null\n"
			 "	-s program         The path of X Server, default 'X'\n"
			 "	-u username        The user to login, default null (Not login)\n"
			 "	-l login           Same as -u\n"
			 "	-n                 Do not use the su command of system (default used)\n"
			 "	-A                 Use MIT-MAGIC-COOKIE-1 XSecurity\n"
			 "	-g                 Use greeter mode (After session exited restart it)\n"
			 "	-h                 Show this usage\n"
			 "\n SERVER OPTIONS\n"
			 "	The additional options to X server. e.g. -depth x\n"
			 "\n EXAMPLES\n"
			 "	%s -d :0 -c gnome-session -u my_user_name -n\n"
			 "	%s -d :2 -v vt3 -c xterm -r metacity\n"
			 "	%s -c /path/to/myde_init.sh -u my_user_name -n -A\n"
			 "	%s -c /usr/lib/mydm/mydm-gtk-demo-greeter/mydm-gtk-demo-greeter -g\n"
			 , argv[0], argv[0], argv[0], argv[0], argv[0]);
			exit(0);
			break;
		}
	}

	if (greeter_mode && arg_use_xauth) {
		mydm_print("-g and -A can not coexist between, XSecurity should be provided by greeter.\n");
		exit(1);
	}

work_start:
	xstart = 0;
	xsvrpid = 0;
	xclipid = 0;

	my_signal(SIGQUIT, SIG_IGN, 1);
	my_signal(SIGTSTP, SIG_IGN, 1);

	my_signal_cld_reset(SIGHUP, SIG_IGN, 1);
	my_signal_cld_reset(SIGTERM, sig_term, 1);
	my_signal_cld_reset(SIGCHLD, sig_child, 1);
	my_signal_cld_reset(SIGUSR1, sig_user1, 1);

	my_signal_cld_ign(SIGINT, sig_term, 1);

	/* when arg_user is null, it will use the uid of process's */
	if (arg_use_xauth && 
	    (xauth_magic_cookie_prepare_filename(arg_user, NULL) < 0))
		err_quit("xauth_magic_cookie_prepare_filename");

	block_signal(SIGCHLD);
	INIT_PIPE();
	if ((pid = fork()) < 0)
		err_quit("fork");

	if (pid == 0) {
		unblock_signal(SIGCHLD);

		my_signal_cld_ign(SIGINT, SIG_IGN, 1);

		my_signal(SIGTTIN, SIG_IGN, 1);
		my_signal(SIGTTOU, SIG_IGN, 1);

		my_signal(SIGUSR1, SIG_IGN, 1);

		exec_xserver(arg_xserver, arg_display, arg_vt, arg_use_xauth, argc - optind, &argv[optind]);
		mydm_print("exec X server error: %s\n", strerror(errno));

		/* prevents restart of the session on greeter mode */
		TELL_PARENT();

		exit(1);
	}

	if (WAIT_CHILD() < 0)
		greeter_mode = 0;

	xsvrpid = pid;
	unblock_signal(SIGCHLD);

	/* Wait for X Server wake up me. */
	while (!xstart)
		pause();

	my_signal_cld_reset(SIGUSR1, SIG_IGN, 1);

	/* Can the display connect? */
	if ((display = XOpenDisplay(arg_display)) == NULL) {
		mydm_print("Cannot open display\n");

		/* prevents restart of the session on greeter mode */
		greeter_mode = 0;

		killxsvr();
		while (1) pause();
	}

	XCloseDisplay(display);

	/* when arg_user is null, it will use the uid of process's */
	if (arg_use_xauth && 
	    (xauth_magic_cookie_gen(arg_display, arg_user) < 0)) {
		mydm_print("Cannot generate a magic cookie\n");

		/* prevents restart of the session on greeter mode */
		greeter_mode = 0;

		killxsvr();
		while (1) pause();
	}

	setenv("DISPLAY", arg_display, 1);

	block_signal(SIGCHLD);
	INIT_PIPE();
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
				mydm_print("exec %s error: %s\n", arg_run, strerror(errno));

				/* prevents restart of the session on greeter mode */
				TELL_PARENT();

				kill(getppid(), SIGTERM);
				exit(1);
			}
		}

		exec_try_login_user(arg_user, arg_xclient, no_system_su);
		if (cpid)
			kill(cpid, SIGTERM);
		mydm_print("exec X client '%s' error: %s\n", arg_xclient, strerror(errno));

		/* prevents restart of the session on greeter mode */
		TELL_PARENT();

		exit(1);
	}

	if (WAIT_CHILD() < 0)
		greeter_mode = 0;

	xclipid = pid;
	unblock_signal(SIGCHLD);

	while (repause)
		pause();

	mydm_print("Restarting a session...\n");
	repause = 1;
	goto work_start;

	exit(0);
}
