// *************************
// Ben's Implemented SHell
//         bish
// *************************

#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sstream>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"
#include "util_fns.h"
using namespace std;

int main(int argc, char **argv){

  signal(SIGINT, ctrlCHandler);

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
    cout << "history file not found. creating `~/.bish_history`." << endl;
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
    // parse line
    vector<string> wfwe = split(line, ';');
    for (auto it: wfwe) {

      command *cmd = parse(split(it.c_str()));
      // print_cmd(cmd);
      // clear line var
      free(line);
      line = (char*)NULL;

      // COMMANDS that do something with the line before fork/exec
      if (strcmp(cmd->cmds[0]->args[0], "!") == 0) {
        line = history_get(where_history())->line;
        cout << line << endl;
        cmd = parse(split(line));
      }

      else if (strcmp(cmd->cmds[0]->args[0], ".") == 0 || strcmp(cmd->cmds[0]->args[0], "source") == 0) {
        int dotsrcfile = open(cmd->cmds[0]->args[1], O_RDONLY);
        if (dotsrcfile < 0) {
          perror("dotsrcfile");
          continue;
        }
        // do the thing to read the files
      }

      // COMMANDS that skip fork/exec
      // http://www.linuxquestions.org/questions/programming-9/making-a-c-shell-775690/
      if (strcmp(cmd->cmds[0]->args[0], "exit") == 0) break;

      // handle chdir
      else if (strcmp(cmd->cmds[0]->args[0], "cd") == 0) {
        if (cmd->cmds[0]->args[1] == NULL) {
          if (chdir(homedir) < 0) perror("chdir");
        }
        else {
          if (chdir(cmd->cmds[0]->args[1]) < 0) perror("chdir");
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
          int infd = 0, outfd = 1;

          // io redirection
          if (cmd->cmds[0]->outfile != "") {
            outfd = open(cmd->cmds[0]->outfile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
            if (outfd < 0) {
              perror("outfile");
              exit(0);
            }
          }
          if (cmd->cmds[0]->infile != "") {
            infd = open(cmd->cmds[0]->infile.c_str(), O_RDONLY);
            if (infd < 0) {
              perror("infile");
              exit(0);
            }
          }

          bishexec(cmd, infd, outfd);
          exit(1);

        } // end child
        // parent waits for kid to die
        else {

          int status;

          if (!cmd->background){
            do {
              if (waitpid(kidpid, &status, WUNTRACED | WCONTINUED) == -1) {
                perror("waitpid");
                exit(1);
              }
              if (WIFEXITED(status)) {
                cout << "(" << WEXITSTATUS(status) << "):";
              } else if (WIFSIGNALED(status)) {
                cout << endl << "killed by signal " << WTERMSIG(status) << endl;
              } else if (WIFSTOPPED(status)) {
                cout << endl << "stopped by signal " << WSTOPSIG(status) << endl;
              } else if (WIFCONTINUED(status)) {
                cout << endl << "continued" << endl;
              }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
          }

        } // end parent

      } // end execute area

      // reset args array for the next prompt
      delete cmd;

    } // end ';' split
  } // end main while loop
  cout << endl;
  if (write_history(histpath.c_str())) perror("write_history");

  return 0;
}
