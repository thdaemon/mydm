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
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>

#define _GNU_SOURCE
#include <crypt.h>

#include <gtk/gtk.h>

#include "../../su.h"


#define GLADE_FILE_NAME "/usr/lib/mydm/mydm-gtk-demo-greeter/mydm-gtk-demo-greeter.glade"
#define XINIT_FILE_NAME "/usr/lib/mydm/mydm-gtk-demo-greeter/deinit"

GtkWidget *entry_username, *entry_password;
GtkWidget *window;

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

/* window destroy */
void on_window1_destroy()
{
	gtk_main_quit();
}

/* login button click */
void on_button1_clicked()
{
	const char *username, *password;

	username = gtk_entry_get_text(GTK_ENTRY(entry_username));
	password = gtk_entry_get_text(GTK_ENTRY(entry_password));

	if (auth_user(username, password) < 0) {
		mesbox("No such user or password wrong.");
		return;
	}

	if (switch_user(username) < 0) {
		mesbox("Password ok but can not login the user.");
		return;
	}

	execlp(XINIT_FILE_NAME, XINIT_FILE_NAME, NULL);
	gtk_main_quit();
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

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(builder);

	gtk_widget_show(window);
	gtk_main();
	return 0;
}
