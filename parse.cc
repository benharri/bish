// Parser stuff for bish

#include <string>
#include <vector>
#include <iostream>
#include "parse.h"
#include "util_fns.h"
using namespace std;



void print_cmd(command *cmd) {
  if (cmd->background) cout << "backgroud: true" << endl;
  if (cmd->piping) cout << "piping to: " << cmd->piping << endl;
  if (cmd->cmds[0]->infile != "") cout << "infile: " << cmd->cmds[0]->infile << endl;
  if (cmd->cmds[0]->outfile != "") cout << "outfile: " << cmd->cmds[0]->outfile << endl;
}



command *parse(vector<string> args) {

  command *parseinfo = new command();
  bool in_flag = false, out_flag = false;//, piping_flag = false;
  vector<vector<string>> cmd;
  int num_cmds = 0;

  for (auto it: args) {
    if (it == "<") {
      in_flag = true;
      continue;
    }
    else if (in_flag) {
      in_flag = false;
      parseinfo->cmds[num_cmds]->infile = it;
    }

    else if (it == ">") {
      out_flag = true;
      continue;
    }
    else if (out_flag) {
      out_flag = false;
      parseinfo->cmds[num_cmds]->outfile = it;
    }

    else if (it == "|") {
      // piping_flag = true;
      // cmd.push_back();
      num_cmds++;
      continue;
    }
    // else if (piping_flag) {
    //   // piping_flag = false;
    //   parseinfo->piping = true;
    // }

    else if (it == args.back() && it == "&") {
      parseinfo->background = true;
    }

    else {
      cmd[num_cmds].push_back(it);
    }
  }

  int i = 0;
  for (auto it: cmd) {
    parseinfo->cmds[i]->args = v_to_cpp(cmd[0]);

  }
  parseinfo->num_cmds = num_cmds;
  return parseinfo;
}
