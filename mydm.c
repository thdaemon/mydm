#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <X11/Xlib.h>

int xstart = 0;
pid_t xsvrpid = 0, xclipid = 0;

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

void err_quit(char* msg)
{
	perror(msg);
	exit(1);
}

int killxsvr()
{
	fprintf(stderr, "Killing X Server\n");
	return kill(xsvrpid, SIGTERM);
}

void sig_user1(int signo)
{
	fprintf(stderr, "Catch SIGUSR1\n");
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
		fprintf(stderr, "X Server exited\n");
		exit(0);
	}

	if (pid == xclipid) {
		killxsvr();
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	pid_t pid;
	Display* display;

	my_signal(SIGUSR1, sig_user1, 1);
	my_signal(SIGCHLD, sig_child, 1);

	if ((pid = fork()) < 0)
		err_quit("fork");

	if (pid == 0) {
		my_signal(SIGTTIN, SIG_IGN, 0);
		my_signal(SIGTTOU, SIG_IGN, 0);

		my_signal(SIGUSR1, SIG_IGN, 0);

		execlp("X", "X", ":0", "vt7", "-nolisten", "tcp", 0);
		fprintf(stderr, "Exec X Server Error\n");

		exit(1);
	}

	xsvrpid = pid;

  /* Wait for X Server wake up me. */
	while (!xstart)
		pause();

	my_signal(SIGUSR1, SIG_IGN, 1);

  /* Can the display connect? */
	if ((display = XOpenDisplay(":0")) == NULL) {
		fprintf(stderr, "Cannot open display\n");
		killxsvr();
		exit(1);
	}

	XCloseDisplay(display);

	setenv("DISPLAY", ":0", 1);
	if ((pid = fork()) < 0) {
		killxsvr();
		err_quit("fork");
	}

	if (pid == 0) {
    /*
     * 'user' is your user name
     * and if you want to start lxde, you can use 'startlxde' replace 'startxfce4' :-)
     */
		execlp("su", "su", "-", "user", "-c", "exec startxfce4", 0);
		fprintf(stderr, "Exec X Client Error\n");
		killxsvr();
		exit(1);
	}

	xclipid = pid;

	while (1)
		pause();

	exit(0);
}
