// bish util fns

#include <vector>
#include <string>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
// #include "util_fns.h"
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


