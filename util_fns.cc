// bish util fns

#include <vector>
#include <string>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "parse.h"
#include "util_fns.h"
using namespace std;


void ctrlCHandler(int sig) {
  cout << endl << "\e[34mbish:\e[92m" << get_current_dir_name() << "\e[34m:$\e[0m ";
}

// util methods
vector<string> split(const char *str, char c) {
  vector<string> result;
  do {
    const char *begin = str;
    while (*str != c && *str) str++;
    string tmp = string(begin, str);
    if (tmp != "") result.push_back(tmp);
  } while (0 != *str++);
  return result;
}

char** v_to_cpp(vector<string> vargs) {
  char** args = new char *[vargs.size()+1];
  // http://stackoverflow.com/questions/26032039/convert-vectorstring-into-char-c
  for (size_t i = 0; i < vargs.size(); ++i) {
    args[i] = new char[vargs[i].size() + 1];
    strcpy(args[i], vargs[i].c_str());
  }
  args[vargs.size()] = NULL;
  return args;
}


void bishexec(simple_command* cmd, int infd, int outfd) {

  if (outfd != 1) {
    if (dup2(outfd, 1) == -1) {
      perror("dup2 outfile");
      exit(0);
    }
  }
  if (infd != 0) {
    if (dup2(infd, 0) == -1) {
      perror("dup2 infile");
      exit(0);
    }
  }

  // try to run it as is
  execvp(cmd->vargs[0].c_str(), v_to_cpp(cmd->vargs));

  // search the path
  // vector<string> path = split(getenv("PATH"), ':');
  // stringstream curr;

  // for (auto iter: path) {
  //   curr.str("");
  //   curr << iter << "/" << cmd->vargs[0];
  //   execv(curr.str().c_str(), v_to_cpp(cmd->vargs));
  // }
  // nothing found here...
  cout << "that's not a command, bish" << endl;
}
