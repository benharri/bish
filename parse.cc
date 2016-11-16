// Parser stuff for bish

#include <string>
#include <vector>
#include <iostream>
#include "parse.h"
#include "util_fns.h"
using namespace std;



void print_cmd(command *cmd) {
  int i = 0;
  cout << "-----------------------------" << endl;
  for (auto cmd_iter: cmd->cmds) {
    if (cmd->background) cout << "backgroud: true" << endl;
    cout << "command " << ++i << endl;
    if (cmd_iter.infile != "") cout << "infile: " << cmd_iter.infile << endl;
    if (cmd_iter.outfile != "") cout << "outfile: " << cmd_iter.outfile << endl << endl;
    cout << ">>\t";
    for (auto scmd_iter: cmd_iter.vargs) {
      cout << "\"" << scmd_iter << "\" ";
    }
    cout << endl << endl;
  }
  cout << "-----------------------------" << endl;

}



command *parse(vector<string> args) {

  command *parsed_command = new command();
  bool in_flag = false, out_flag = false;
  simple_command *current_simple = new simple_command();

  for (auto iter: args) {
    if (iter == "<") {
      in_flag = true;
      continue;
    }
    else if (in_flag) {
      in_flag = false;
      current_simple->infile = iter;
    }

    else if (iter == ">") {
      out_flag = true;
      continue;
    }
    else if (out_flag) {
      out_flag = false;
      current_simple->outfile = iter;
    }

    else if (iter == "|") {
      parsed_command->cmds.push_back(*current_simple);
      delete current_simple;
      current_simple = new simple_command();
      continue;
    }

    else if (iter == args.back() && iter == "&") {
      parsed_command->background = true;
    }

    else {
      current_simple->vargs.push_back(iter);
    }
  }
  parsed_command->cmds.push_back(*current_simple);
  return parsed_command;
}
