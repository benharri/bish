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
  prompt << "bish\e[92m" << get_current_dir_name() << " $\e[0m ";

  // set up history
  using_history();
  read_history("~/.bish_history");

  // register ctrl c handler
  // signal(SIGINT, ctrl_c_handler);

  while (!done) {

    prompt.str("");
    prompt << "bish\e[92m" << get_current_dir_name() << " $\e[0m ";
    line = (char*)NULL;
    line = readline(prompt.str().c_str());
    if (!line) {
      // done = 1;
      break;
    }

    // if (line[0]) {
    //   char *expansion;
    //   int result;
    //   result = history_expand(line, &expansion);
    //   if (result) {
    //     fprintf(stderr, "%s\n", expansion);
    //   }
    //   if (result < 0 || result == 2) {
    //     free(expansion);
    //     continue;
    //   }
    //   add_history(expansion);
    //   strncpy(line, expansion, sizeof(line) - 1);
    //   free(expansion);
    // } else done = 1;

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
        // int e =
        execv(args[0], args);
        // cout << "return from exec on fullpath: " << e << endl;
        // search the path
        // if (e == -1) {
          cout << "Searching path";
          for (auto it: path) {
            char* searchpath;
            searchpath = strcpy(searchpath, (char*)it.c_str());
            strcat(searchpath, "/");
            strcat(searchpath, args[0]);
            cout << searchpath << endl;
            cout << "arg0" << args[0] << endl;
            execv(searchpath, args);
          }
        // }

        // nothing found here...
        perror("bish");
        exit(1);
      }
      // parent waits for kid to die
      else {
        int status;
        if (wait(&status) == -1) perror("wait error");
        if (WIFSIGNALED(status) != 0) {
          printf("Child process ended because of signal %d\n", WTERMSIG(status));
        }
        else if (WIFEXITED(status) != 0) {
          printf("Child process did not end normally; status = %d\n", WEXITSTATUS(status));
        }
        else {
            // Program exited abnormally
          cout << "Program exited abnormally" << endl;
        }
      }
    }

    delete[] args;
  }
  write_history("~/.bish_history");
  return 0;
}
