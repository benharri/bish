// Ben Harris

#ifndef _UTIL_FNS_H_
#define _UTIL_FNS_H_

#include <string>
#include <string.h>
#include <vector>
using namespace std;

// method definitions
void ctrlCHandler(int sig);
vector<string> split(const char *str, char c = ' ');
char** v_to_cpp(vector<string> vargs);
void bishexec(simple_command* cmd, int infd, int outfd);
bool remap_pipe_stdin_stdout(int rpipe, int wpipe);
int expand_and_execute (const char *program, char **options);
int bish_expandexec(simple_command* cmd);
void dup_io(int infd, int outfd, bool ispipe = false);

#endif
