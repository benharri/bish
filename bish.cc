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

char** parse_args(vector<string> vargs) {
  char** args = new char *[vargs.size()+1];
  // http://stackoverflow.com/questions/26032039/convert-vectorstring-into-char-c
  for (size_t i = 0; i < vargs.size(); ++i) {
    args[i] = new char[vargs[i].size() + 1];
    strcpy(args[i], vargs[i].c_str());
  }
  // use executable name as first
  // strcpy(args[0], split(args[0], '/'),back().c_str());
  args[vargs.size()] = nullptr;
  return args;
}



int main(int argc, char **argv){
  string line;
  int status;
  vector<string> path = split(getenv("PATH"), ':');

  cout << "\e[92m" << get_current_dir_name() << " $\e[0m ";

  while (getline(cin, line)) {
    char **args = parse_args(split(line.c_str()));

    // handle exit
    // http://www.linuxquestions.org/questions/programming-9/making-a-c-shell-775690/
    if (strcmp(args[0], "exit") == 0) return 0;
    // handle chdir
    if (!strcmp(args[0], "cd")) {
      if (args[1] == NULL) {
        if (chdir("/") < 0) perror("chdir");
      }
      else {
        if (chdir(args[1]) < 0) perror("chdir");
      }
    }
    // process the line.
    else {
      pid_t kidpid = fork();
      // fork error
      if (kidpid < 0) {
        perror("fork");
        return -1;
      }
      // run it
      // also check the path for things
      else if (kidpid == 0){
        // try to run it as is
        int e = execv(args[0], args);
        // cout << "return from exec on fullpath: " << e << endl;
        // search the path
        if (e < 0) {
          // cout << "Searching path";
          for (auto it: path) {
            char* searchpath = (char*)it.c_str();
            strcat(searchpath, "/");
            strcat(searchpath, args[0]);
            // cout << searchpath << endl;
            execv(searchpath, args);
          }
        }

        // nothing found here...
        perror("child process");
      }
      // parent waits for kid to die
      else {

        wait(&status);
        if (WIFEXITED(status)) {
          int exstatus = WEXITSTATUS(status);
          // if (exstatus == 0) {
            // Program succeeded
            cout << "Status:" << exstatus;
          // }
          // else {
            // Program failed but exited normally
            // cout << "";
          // }
        }
        else {
            // Program exited abnormally
          cout << "Program exited abnormally" << endl;
        }
      }
    }

    delete[] args;
    cout << "\e[92m" << get_current_dir_name() << " $\e[0m ";
  }
  return 0;
}
