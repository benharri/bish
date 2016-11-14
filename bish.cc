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


    int done = 0;
    while (!done) {

        prompt.str("");
        prompt << "\e[34mbish:\e[92m" << get_current_dir_name() << "\e[34m:$\e[0m ";

        line = readline(prompt.str().c_str());
        if (line == NULL) break;
        if (strcmp(line, "") == 0) continue;
        if (line && *line) add_history (line);

        // handle multiple commands w/ semicolon
        vector<string> wfwe = split(line, ';');
        for (auto it: wfwe) {

            command *cmd = parse(split(it.c_str()));
            int num_cmds = cmd->cmds.size();
            // debug info
            print_cmd(cmd);

            // clear line var
            free(line);
            line = (char*)NULL;

            if (cmd->cmds[0].vargs[0] == "exit") {
                done = 1;
                break;
            }
            if (cmd->cmds[0].vargs[0] == "cd") {
                if (cmd->cmds[0].vargs[1] == "") {
                    if (chdir(homedir) < 0) perror("chdir");
                } else {
                    if (chdir(cmd->cmds[0].vargs[1].c_str()) < 0) perror("chdir");
                }
            }

            // int n;
            // bish_expandexec(&cmd->cmds[0]);
            int in = 0, fd[2];
            for (int i = 0; i < num_cmds-1; i++) {
                pipe(fd);
                dup_io(in, fd[1]);
                bish_expandexec(&cmd->cmds[i]);
                // close(fd[1]);
                in = fd[0];
            }
            if (in != 0) {
                dup_io(in, 1);
                bish_expandexec(&cmd->cmds[num_cmds]);
            }

            //     // else { // do fork/exec
            // pid_t kidpid = fork();
            // // fork error
            // if (kidpid < 0) {
            //     perror("fork");
            //     return -1;
            // }

            // // run it
            // // also check the path for things
            // else if (kidpid == 0){
            //     int infd = 0, outfd = 1;

            //     if (num_cmds > 1) {
            //         int pipefd[2];
            //         pipe(pipefd);
            //         pid_t nextkidpid = fork();
            //         if (nextkidpid < 0) {
            //             perror("fork");
            //             return -1;
            //         }
            //         else if (nextkidpid == 0) {
            //             close(pipefd[0]);
            //             outfd = pipefd[1];
            //             bishexec(&curr, infd, outfd);
            //         }
            //     }

            //     // io redirection
            //     if (curr.outfile != "") {
            //         outfd = open(curr.outfile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
            //         if (outfd < 0) {
            //             perror("outfile");
            //             exit(0);
            //         }
            //     }
            //     if (curr.infile != "") {
            //         infd = open(curr.infile.c_str(), O_RDONLY);
            //         if (infd < 0) {
            //             perror("infile");
            //             exit(0);
            //         }
            //     }

            //     bishexec(&curr, infd, outfd);
            //     exit(1);

            // } // end child
            // // parent waits for kid to die
            // else {

            //     int status;

            //     if (!cmd->background){
            //         do {
            //             if (waitpid(kidpid, &status, WUNTRACED | WCONTINUED) == -1) {
            //                 perror("waitpid");
            //                 exit(1);
            //             }
            //             if (WIFEXITED(status)) {
            //                 cout << "(" << WEXITSTATUS(status) << "):";
            //             } else if (WIFSIGNALED(status)) {
            //                 cout << endl << "killed by signal " << WTERMSIG(status) << endl;
            //             } else if (WIFSTOPPED(status)) {
            //                 cout << endl << "stopped by signal " << WSTOPSIG(status) << endl;
            //             } else if (WIFCONTINUED(status)) {
            //                 cout << endl << "continued" << endl;
            //             }
            //         } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            //     }

            // } // end parent


            // COMMANDS that do something with the line before fork/exec
            // if (strcmp(cmd->cmds[0]->args[0], "!") == 0) {
            //   line = history_get(where_history())->line;
            //   cout << line << endl;
            //   cmd = parse(split(line));
            // }

            // else if (strcmp(cmd->cmds[0]->args[0], ".") == 0 || strcmp(cmd->cmds[0]->args[0], "source") == 0) {
            //   int dotsrcfile = open(cmd->cmds[0]->args[1], O_RDONLY);
            //   if (dotsrcfile < 0) {
            //     perror("dotsrcfile");
            //     continue;
            //   }
            //   // do the thing to read the files
            // }



            // reset args array for the next prompt
            delete cmd;

        } // end ';' split
    } // end main while loop
    cout << endl;
    if (write_history(histpath.c_str())) perror("write_history");

    return 0;
}
