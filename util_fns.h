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
void bishexec(command* cmd, int infd, int outfd);

#endif
