#include <string>
#include <vector>
#include <iostream>
#include "parse.h"
#include "util_fns.h"
using namespace std;



void print_cmd(command *cmd) {
  cout << "args: " << endl;
  int i = 0;
  while (cmd->args[i]) {
    int j = 0;
    while (cmd->args[i][j]) {
      cout << "\t" << i << ", " << j << ": " << cmd->args[i][j] << endl;
    }
  }

  if (cmd->background) cout << "backgroud: true" << endl;
  else cout << "background: false" << endl;

  if (cmd->piping.c_str()) cout << "piping to: " << cmd->piping << endl;
  if (cmd->infile.c_str()) cout << "infile: " << cmd->infile << endl;
  if (cmd->outfile.c_str()) cout << "outfile: " << cmd->outfile << endl;
}



command *parse(vector<string> args) {
  command *parseinfo = new command();
  bool in_flag = false, out_flag = false, piping_flag = false;
  vector<string> cmd;

  for (auto it: args) {
    if (it == "<") {
      in_flag = true;
      continue;
    }
    else if (in_flag) {
      in_flag = false;
      parseinfo->infile = it;
    }

    else if (it == ">") {
      out_flag = true;
      continue;
    }
    else if (out_flag) {
      out_flag = false;
      parseinfo->outfile = it;
    }

    else if (it == "|") {
      piping_flag = true;
      continue;
    }
    else if (piping_flag) {
      piping_flag = false;
      parseinfo->piping = it;
    }

    else if (it == args.back() && it == "&") {
      parseinfo->background = true;
    }

    else cmd.push_back(it);
  }
  parseinfo->args[0] = v_to_cpp(cmd);
  return parseinfo;
}
