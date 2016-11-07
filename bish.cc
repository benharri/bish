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
using namespace std;

// util methods
vector<string> split(const char *str, char c = ' ') {
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

// command struct
struct command {
  char*** args;
  bool background;
  string piping;
  string infile;
  string outfile;
};

command parse(vector<string> args) {
  command parseinfo;
  bool in_flag = false, out_flag = false, piping_flag = false;
  vector<string> cmd;

  for (auto it: args) {
    if (it == "<") in_flag = true;
    if (in_flag) {
      in_flag = false;
      parseinfo.infile = it;
    }

    if (it == ">") out_flag = true;
    if (out_flag) {
      out_flag = false;
      parseinfo.outfile = it;
    }

    if (it == "|") piping_flag = true;
    if (piping_flag) {
      piping_flag = false;
      parseinfo.piping = it;
    }

    if (it == args.back() && it == "&") {
      parseinfo.background = true;
    }

    cmd.push_back(it);
  }
  parseinfo.args[0] = v_to_cpp(cmd);
  return parseinfo;
}



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

    char **args = v_to_cpp(split(line));
    free(line);
    line = (char*)NULL;

    // http://www.linuxquestions.org/questions/programming-9/making-a-c-shell-775690/
    if (strcmp(args[0], "exit") == 0) break;
    // handle chdir
    else if (strcmp(args[0], "cd") == 0) {
      if (args[1] == NULL) {
        if (chdir(homedir) < 0) perror("chdir");
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
    delete[] args;

  }

  printf("\n");
  if (write_history(histpath.c_str())) perror("write_history");

  return 0;
}
