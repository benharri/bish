#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/types.h>
using namespace std;

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

void parse_args(vector<string> vargs, char*** args) {
  *args = (char**)malloc((vargs.size()+1)*sizeof(char*));
  int i = 0;
  for(auto it = vargs.begin(); it < vargs.end(); ++it, i++) {
    string tmp = *it;
    *args[i] = (char *)tmp.c_str();
    // cout << *it << endl;
  }
  *args[vargs.size()] = NULL;
}

// main
int main(int argc, char **argv){
  char **args;
  string line;
  int status;

  vector<string> path = split(getenv("PATH"), ':');

  cout << get_current_dir_name() << "$ ";
  while (getline(cin, line)) {

    parse_args(split(line.c_str()), &args);

    // http://www.linuxquestions.org/questions/programming-9/making-a-c-shell-775690/
    if (strcmp(args[0], "exit") == 0) return 0;
    if (!strcmp(args[0], "cd")) {
      if (args[1] == NULL) {
        if (chdir("/") < 0) perror("chdir");
      }
      else {
        if (chdir(args[1]) < 0) perror("chdir");
      }
    } else {
      pid_t kidpid = fork();
      if (kidpid < 0) {
        perror("fork");
        return -1;
      }
      else if (kidpid == 0){
        cout << args[0] << endl;
        for (auto it: path) {
          string searchpath = string() + it + "/" + string(args[0]);
          cout << searchpath << endl;
          execv(searchpath.c_str(), args);
        }
        perror("child process");
      }
      else {
        wait(&status);
      }
    }

    cout << get_current_dir_name() << "$ ";
    free(args);
  }
  return 0;
}
