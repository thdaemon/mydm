/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is an opensource (free) software
 */

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pwd.h>
#include <shadow.h>

#define _GNU_SOURCE
#include <crypt.h>

#include <gtk/gtk.h>

#include "../../config.h"
#include "../../su.h"


#define GLADE_FILE_NAME "/usr/lib/mydm/mydm-gtk-demo-greeter/mydm-gtk-demo-greeter.glade"
#define XINIT_FILE_NAME "/usr/lib/mydm/mydm-gtk-demo-greeter/deinit"
#define DERUN_FILE_NAME "/usr/lib/mydm/mydm-gtk-demo-greeter/desktop_run"

GtkWidget *window;
GtkWidget *entry_username, *entry_password;
GtkWidget *combo;
GtkWidget *chkb_xauth;

static int auth_user(const char *username, const char *password)
{
	struct passwd *pw;
	struct spwd *sp;
	char *crypted, *pass;

	pw = getpwnam(username);
	endpwent();

	if (!pw) {
		errno = ENOENT;
		return -1;
	}

	sp = getspnam(pw->pw_name);
	endspent();

	pass = sp ? sp->sp_pwdp : pw->pw_passwd;

	if ((crypted = crypt(password, pass)) == NULL)
		return -1;
	
	if (strcmp(crypted, pass) != 0) {
		errno = EACCES;
		return -1;
	}

	return 0;
}

void mesbox(const gchar *message)
{
	GtkWidget *msgdialog;
	//gtk_widget_set_sensitive(window, FALSE);
	msgdialog = gtk_message_dialog_new(GTK_WINDOW(window),
		                           GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
		                           GTK_MESSAGE_ERROR,
		                           GTK_BUTTONS_CLOSE,
		                           "%s", message);
	gtk_dialog_run(GTK_DIALOG(msgdialog));
	gtk_widget_destroy(msgdialog);
	//gtk_widget_set_sensitive(window, TRUE);
}

#ifdef CONFIG_ENABLE_XSEC
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

int xauth_magic_cookie_gen(const char* username)
{
	pid_t pid;
	int status;
	struct passwd *pwd;
	char *filename;

	if ((pwd = getpwnam(username)) == NULL)
		return -1;

	if ((filename = getenv("XAUTHORITY")) == NULL)
		return -1;

	block_signal(SIGCHLD);
	if ((pid = fork()) < 0)
		return -1;

	if (pid == 0) {
		unblock_signal(SIGCHLD);

		execlp("xauth", "xauth", "generate", getenv("DISPLAY"), "MIT-MAGIC-COOKIE-1", NULL);
		return -1;
	}

	while (waitpid(pid, &status, 0) != pid) {
		if (errno != EINTR) {
			return -1;
		}
	}

	unblock_signal(SIGCHLD);

	if ((!WIFEXITED(status)) || (WEXITSTATUS(status) != 0))
		return -1;
	if (chmod(filename, S_IRUSR|S_IWUSR) < 0)
		return -1;

	if (chown(filename, pwd->pw_uid, pwd->pw_gid) < 0)
		return -1;

	return 0;
}
#else
int xauth_magic_cookie_gen()
{
	mesbox("XSecurity: this feature is not compiled.");
	return -1;
}
#endif /* CONFIG_ENABLE_XSEC */

/* window destroy */
void on_window1_destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

/* login button click */
void on_button1_clicked(GtkWidget *widget, gpointer data)
{
	const char *username, *password;

	username = gtk_entry_get_text(GTK_ENTRY(entry_username));
	password = gtk_entry_get_text(GTK_ENTRY(entry_password));

	if (auth_user(username, password) < 0) {
		mesbox("No such user or password wrong.");
		return;
	}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkb_xauth))) {
		if (xauth_magic_cookie_gen(username) < 0) {
			mesbox("XSecurity: cannot generate a magic cookie.");
			gtk_main_quit();
			return;
		}
	}

	if (switch_user(username) < 0) {
		mesbox("Password ok but can not login the user.");
		return;
	}

	if (gtk_combo_box_get_active(GTK_COMBO_BOX(combo)) == 0) {
		execlp(XINIT_FILE_NAME, XINIT_FILE_NAME, NULL);
	} else {
		char desktopfile[2048];
		snprintf(desktopfile, sizeof(desktopfile), "/usr/share/xsessions/%s",
		         gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo)));
		execlp(DERUN_FILE_NAME, DERUN_FILE_NAME, desktopfile, NULL);
	}

	gtk_main_quit();
}

int combo_additem(GtkWidget *combobox)
{
	DIR* dir;
	struct dirent* dent;

	if ((dir = opendir("/usr/share/xsessions/")) == NULL)
		return -1;

	while ((dent = readdir(dir)) != NULL) {
		if ((strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..") == 0))
			continue;

		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combobox), NULL, dent->d_name);
	}

	closedir(dir);
	return 0;
}

int main(int argc, char *argv[])
{
	GtkBuilder *builder;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, GLADE_FILE_NAME, NULL);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));

	entry_username = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
	entry_password = GTK_WIDGET(gtk_builder_get_object(builder, "entry2"));

	combo = GTK_WIDGET(gtk_builder_get_object(builder, "comboboxtext1"));
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, "DE Init Script");
	combo_additem(combo);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

	chkb_xauth = GTK_WIDGET(gtk_builder_get_object(builder, "checkbutton1"));

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(builder);

	gtk_widget_show(window);
	gtk_main();
	return 0;
}
