/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is an opensource (free) software
 */

#ifndef __XSEC_H_
#define __XSEC_H_

int xauth_magic_cookie_prepare_filename(const char *username, const char *xauth_file);
int xauth_magic_cookie_gen(const char *display, const char *username);

#endif /* __XSEC_H_ */
