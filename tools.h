#ifndef __TOOLS_H_
#define __TOOLS_H_

void err_quit(char* msg);
void* my_signal(int signum, void* handler, int restartsyscall);
int unblock_signal(int signum);
int block_signal(int signum);
int exec_try_login_user(char* username, char* file);

#define my_signal_cld_reset my_signal

#endif /* __TOOLS_H_ */
