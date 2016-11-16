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
            // print_cmd(cmd);

            // clear line var cause we already parsed it
            free(line);
            line = (char*)NULL;

            if (cmd->cmds[0].vargs[0] == "exit") {
                done = 1;
                break;
            }

            // COMMANDS that do something with the line before fork/exec
            if (cmd->cmds[0].vargs[0] == "!") {
                line = history_get(where_history())->line;
                cout << line << endl;
                cmd = parse(split(line));
            }

            if (num_cmds > 1) {

                int in = 0, fd[2];
                for (int i = 0; i < num_cmds-1; i++) {
                    if (pipe(fd)) perror("pipe");
                    cmd->cmds[i].ispipe = true;
                    cmd->cmds[i].infd = in;
                    cmd->cmds[i].outfd = fd[1];
                    expand_and_execute(&cmd->cmds[i]);
                    close(fd[1]);
                    in = fd[0];
                }
                if (in != 0) {
                    cmd->cmds[num_cmds-1].ispipe = true;
                    cmd->cmds[num_cmds-1].infd = in;
                    cmd->cmds[num_cmds-1].outfd = 1;
                    expand_and_execute(&cmd->cmds[num_cmds-1]);
                }
            }
            else {
                cmd->cmds[0].ispipe = false;
                expand_and_execute(&cmd->cmds[0]);
            }



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
