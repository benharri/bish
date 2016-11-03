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
// volatile sig_atomic_t flag = 0;
// void ctrl_c_handler(int sig) {
//   flag = 1;
// }

// util methods
vector<string> split(const char *str, char c = ' '){
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
  args[vargs.size()] = nullptr;
  return args;
}



int main(int argc, char **argv){
  int done = 0;
  static char* line = (char*)NULL;
  vector<string> path = split(getenv("PATH"), ':');

  // build prompt
  stringstream prompt;
  prompt << "bish:\e[92m" << get_current_dir_name() << " $\e[0m ";

  // set up history
  using_history();
  read_history("~/.bish_history");

  // register ctrl c handler
  // signal(SIGINT, ctrl_c_handler);

  while (!done) {

    prompt.str("");
    prompt << "bish:\e[92m" << get_current_dir_name() << " $\e[0m ";

    line = readline(prompt.str().c_str());
    if (line == NULL) break;
    if (strcmp(line, "") == 0) continue;
    if (line && *line) add_history (line);
    char **args = v_to_cpp(split(line));
    free(line);
    line = (char*)NULL;

    // handle exit
    // http://www.linuxquestions.org/questions/programming-9/making-a-c-shell-775690/
    if (strcmp(args[0], "exit") == 0) {
      done = 1;
      return 0;
      // exit(0);
    }
    // handle chdir
    else if (strcmp(args[0], "cd") == 0) {
      if (args[1] == NULL) {
        if (chdir("~") < 0) perror("chdir");
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
        execv(args[0], args);
        // search the path
        stringstream searchpath;
        for (auto it: path) {
          searchpath.str("");
          searchpath << it << "/" << args[0];
          execv(searchpath.str().c_str(), args);
        }
        // nothing found here...
        printf("that's not a command, bish");
        exit(1);

      }
      // parent waits for kid to die
      else {
        int status;
        pid_t w;
        do {
          w = waitpid(kidpid, &status, WUNTRACED | WCONTINUED);
          if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
          }

          if (WIFEXITED(status)) {
            printf("(%d):", WEXITSTATUS(status));
          } else if (WIFSIGNALED(status)) {
            printf("\nkilled by signal %d\n", WTERMSIG(status));
          } else if (WIFSTOPPED(status)) {
            printf("\nstopped by signal %d\n", WSTOPSIG(status));
          } else if (WIFCONTINUED(status)) {
            printf("\ncontinued\n");
          }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

      }
    }

    delete[] args;
  }
  printf("\n");
  write_history("~/.bish_history");
  return 0;
}
