// *************************
// Ben's Implemented SHell
//         bish
// *************************

#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"
#include "util_fns.h"
using namespace std;

int main(int argc, char **argv){

  stringstream prompt;
  static char* line = (char*)NULL;
  vector<string> path = split(getenv("PATH"), ':');

  const char *homedir;
  if ((homedir = getenv("HOME")) == NULL) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  // set up history
  using_history();
  string histpath = string(homedir) + string("/.bish_history");
  if (read_history(histpath.c_str())) {
    printf("history file not found. creating `~/.bish_history`.\n");
    int hist = open(histpath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    close(hist);
  }


  while ("bish") {

    prompt.str("");
    prompt << "\e[34mbish:\e[92m" << get_current_dir_name() << "\e[34m:$\e[0m ";

    line = readline(prompt.str().c_str());
    if (line == NULL) break;
    if (strcmp(line, "") == 0) continue;
    if (line && *line) add_history (line);

    // char **args = v_to_cpp(split(line));
    command *cmd = parse(split(line));
    print_cmd(cmd);
    // continue;

    free(line);
    line = (char*)NULL;

    // http://www.linuxquestions.org/questions/programming-9/making-a-c-shell-775690/
    if (strcmp(cmd->args[0][0], "exit") == 0) break;
    // handle chdir
    else if (strcmp(cmd->args[0][0], "cd") == 0) {
      if (cmd->args[0][1] == NULL) {
        if (chdir(homedir) < 0) perror("chdir");
      }
      else {
        if (chdir(cmd->args[0][1]) < 0) perror("chdir");
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
        execv(cmd->args[0][0], cmd->args[0]);
        // search the path
        stringstream searchpath;
        for (auto it: path) {
          searchpath.str("");
          searchpath << it << "/" << cmd->args[0][0];
          execv(searchpath.str().c_str(), cmd->args[0]);
        }
        // nothing found here...
        printf("that's not a command, bish\n");
        exit(1);

      }
      // parent waits for kid to die
      else {

        int status;

        do {
          if (waitpid(kidpid, &status, WUNTRACED | WCONTINUED) == -1) {
            perror("waitpid");
            exit(1);
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

    // reset args array for the next prompt
    // delete[] args;
    delete cmd;

  }

  printf("\n");
  if (write_history(histpath.c_str())) perror("write_history");

  return 0;
}
