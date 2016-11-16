// bish util fns

#include <vector>
#include <string>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <wordexp.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/wait.h>
#include "parse.h"
#include "util_fns.h"
using namespace std;


void ctrlCHandler(int sig) {
  cout << endl << "\e[34mbish:\e[92m" << get_current_dir_name() << "\e[34m:$\e[0m ";
}

// util methods
vector<string> split(const char *str, char c) {
  vector<string> result;
  do {
    const char *begin = str;
    while (*str != c && *str) str++;
    string tmp = string(begin, str);
    if (tmp != "") result.push_back(tmp);
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




void check_cmd_io(simple_command *cmd) {
  if (cmd->ispipe) {
    if (!remap_pipe_stdin_stdout(cmd->infd, cmd->outfd)) {
      // perror("dup2-remap_pipe_stdin_stdout");
    }
    // return;
  }
  // else {
    if (cmd->infile != "") {
      cmd->infd = open(cmd->infile.c_str(), O_RDONLY);
      if (cmd->infd < 0) {
        perror("infile");
        return;
      }
      if (dup2(cmd->infd, 0) == -1) {
        perror("dup2 infile");
        exit(0);
      }
    }
    else cmd->infd = 0;

    if (cmd->outfile != "") {
      cmd->outfd = open(cmd->outfile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
      if (cmd->outfd < 0) {
        perror("outfile");
        return;
      }
      if (dup2(cmd->outfd, 1) == -1) {
        perror("dup2 outfile");
        exit(0);
      }
    }
    else cmd->outfd = 1;
  // }
}




// http://www.gnu.org/software/libc/manual/html_node/Wordexp-Example.html
int expand_and_execute (simple_command *cmd) {
  const char *program = cmd->vargs[0].c_str();
  char **options = v_to_cpp(cmd->vargs);
  wordexp_t result;
  pid_t pid;
  int status, i;


  /* Expand the string for the program to run.  */
  switch (wordexp (program, &result, 0)) {
    case 0:     /* Successful.  */
      break;
    case WRDE_NOSPACE:
      /* If the error was WRDE_NOSPACE,
         then perhaps part of the result was allocated.  */
      wordfree (&result);
    default:                    /* Some other error.  */
      return -1;
  }

  /* Expand the strings specified for the arguments.  */
  for (i = 1; options[i] != NULL; i++) {
      if (wordexp (options[i], &result, WRDE_APPEND)) {
          wordfree (&result);
          return -1;
      }
  }

  const char *homedir;
  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }

  if (strcmp(program, "cd") == 0) {
    if (options[1]) {
      if (chdir(result.we_wordv[1]) < 0) perror("chdir");
    } else {
      if (chdir(homedir) < 0) perror("chdir");
    }
    return 0;
  }

  pid = fork();
  if (pid == 0) {
      /* This is the child process.  Execute the command. */
      check_cmd_io(cmd);
      execvp (result.we_wordv[0], result.we_wordv);
      cout << "that's not a command, bish" << endl;
      exit (EXIT_FAILURE);
  }
  else if (pid < 0)
    /* The fork failed.  Report failure.  */
    status = -1;
  else
    /* This is the parent process.  Wait for the child to complete.  */
    if (waitpid (pid, &status, 0) != pid)
      status = -1;

  wordfree (&result);
  return status;
}








#define DUP2CLOSE(oldfd, newfd) (dup2(oldfd, newfd) == 0  &&  close(oldfd) == 0)
// http://unixwiz.net/techtips/remap-pipe-fds.c.txt
// !!!!!!!
bool remap_pipe_stdin_stdout(int rpipe, int wpipe) {
  /*------------------------------------------------------------------
   * CASE [A]
   *
   * This is the trivial case that probably never happens: the two FDs
   * are already in the right place and we have nothing to do. Though
   * this probably doesn't happen much, it's guaranteed that *doing*
   * any shufflingn would close descriptors that shouldn't have been.
   */
  if ( rpipe == 0  &&  wpipe == 1 ) return true;

  /*----------------------------------------------------------------
   * CASE [B] and [C]
   *
   * These two have the same handling but not the same rules. In case
   * [C] where both FDs are "out of the way", it doesn't matter which
   * of the FDs is closed first, but in case [B] it MUST be done in
   * this order.
   */
  if ( rpipe >= 1  &&  wpipe > 1 ) {
    return DUP2CLOSE(rpipe, 0)
        && DUP2CLOSE(wpipe, 1);
  }


  /*----------------------------------------------------------------
   * CASE [D]
   * CASE [E]
   *
   * In these cases, *one* of the FDs is already correct and the other
   * one can just be dup'd to the right place:
   */
  if ( rpipe == 0  &&  wpipe >= 1 )
    return DUP2CLOSE(wpipe, 1);

  if ( rpipe  >= 1  &&  wpipe == 1 )
    return DUP2CLOSE(rpipe, 0);


  /*----------------------------------------------------------------
   * CASE [F]
   *
   * Here we have the write pipe in the read slot, but the read FD
   * is out of the way: this means we can do this in just two steps
   * but they MUST be in this order.
   */
  if ( rpipe >= 1   &&  wpipe == 0 ) {
    return DUP2CLOSE(wpipe, 1)
        && DUP2CLOSE(rpipe, 0);
  }

  /*----------------------------------------------------------------
   * CASE [G]
   *
   * This is the trickiest case because the two file descriptors  are
   * *backwards*, and the only way to make it right is to make a
   * third temporary FD during the swap.
   */
  if ( rpipe == 1  &&  wpipe == 0 ) {
    const int tmp = dup(wpipe);   /* NOTE! this is not dup2() ! */

    return  tmp > 1
        &&  close(wpipe)   == 0
        &&  DUP2CLOSE(rpipe, 0)
        &&  DUP2CLOSE(tmp,   1);
  }

  /* SHOULD NEVER GET HERE */
  return false;
}
