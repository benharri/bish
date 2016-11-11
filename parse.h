// Ben Harris

#ifndef _PARSE_H_
#define _PARSE_H_

#include <string>
#include <vector>
using namespace std;

// simple_command struct
struct simple_command {
  char** args;
  string infile;
  string outfile;
};
// command struct
struct command {
  simple_command** cmds;
  int num_cmds;
  bool background;
  bool piping;
};

// method definitions
void print_cmd(command *cmd);
command *parse(vector<string> args);

#endif
