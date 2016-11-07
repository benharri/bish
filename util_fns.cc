// bish util fns

#include <vector>
#include <string>
#include <string.h>
#include "util_fns.h"
using namespace std;


// util methods
vector<string> split(const char *str, char c) {
  vector<string> result;
  do {
    const char *begin = str;
    while (*str != c && *str) str++;
    result.push_back(string(begin, str));
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
