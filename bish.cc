#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/types.h>
using namespace std;

// globals
// const char **args;

// util methods
vector<string> split(const char *str, char c = ' '){
  vector<string> result;
  do {
    const char *begin = str;
    while(*str != c && *str) str++;
    result.push_back(string(begin, str));
  } while (0 != *str++);
  return result;
}

void parse_args_vector(vector<string> vargs, char*** args) {
  *args = (char**)malloc((vargs.size()+1)*sizeof(char*));
  int i = 0;
  for(auto it = vargs.begin(); it < vargs.end(); ++it, i++) {
    // string tmp = *it;
    *args[i] = (char *)*it.c_str();
  }
  *args[vargs.size()] = NULL;
}

// main
int main(int argc, char **argv){
  char **args;
  string line;
  int status;

  vector<string> path = split(getenv("PATH"), ':');

  cout << "bish$ ";
  while (getline(cin, line)) {
    parse_args_vector(split(line.c_str()), &args);
    if (fork() == 0){
      cout << "in da child" << endl;
      execv(args[0], args);
      perror("fork child process");
    }
    else {
      cout << "wiating" << endl;
      wait(&status);
    }
    cout << "bish$ ";
    free(args);
  }
  return 0;
}
