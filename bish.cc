#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
using namespace std;

// http://stackoverflow.com/questions/17766550/ctrl-c-interrupt-event-handling-in-linux
volatile sig_atomic_t flag = 0;
void ctrl_c_handler(int sig) {
  flag = 1;
}

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
  args[vargs.size()] = nullptr;
  return args;
}



int main(int argc, char **argv){
  string line;
  vector<string> path = split(getenv("PATH"), ':');

  // register ctrl c handler
  signal(SIGINT, ctrl_c_handler);

  stringstream prompt;
  prompt << "\e[92m" << get_current_dir_name() << " $\e[0m ";

  static char* line_read = (char*)NULL;
  while ((line_read = readline(prompt.str().c_str()))) {

    if (flag) {
      printf("\n");
      continue;
      flag = 0;
    }

    char **args = parse_args(split(line_read));
    if (line_read) {
      free(line_read);
      line_read = (char*)NULL;
    }

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

        int status;
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
    // cout << "\e[92m" << get_current_dir_name() << " $\e[0m ";
  }
  return 0;
}
