/*
 * Copyright Tian Hao <thxdaemon@gmail.com>
 * License: GPLv3
 * It is an opensource (free) software
 */

#ifndef __TOOLS_H_
#define __TOOLS_H_

void err_quit(char* msg);
ssize_t mydm_print(char* fmt, ...);
void* my_signal(int signum, void* handler, int restartsyscall);
int unblock_signal(int signum);
int block_signal(int signum);
int exec_try_login_user(char* username, char* file, int no_system_su);

#define my_signal_cld_reset my_signal
#define my_signal_cld_ign my_signal

#if DEBUG == 1
#define dbglog mydm_print
#else
#define dbglog(...)
#endif

/* WARNING: These functions are not multi-threaded security and asynchronous signal security */
void INIT_PIPE();
void TELL_PARENT();
int WAIT_CHILD();

#endif /* __TOOLS_H_ */
