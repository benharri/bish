// Ben Harris

#ifndef _PARSE_H_
#define _PARSE_H_

#include <string>
#include <vector>
using namespace std;

// simple_command struct
struct simple_command {
  vector<string> vargs;
  // char** args;
  string infile;
  string outfile;
};
// command struct
struct command {
  vector<simple_command> cmds;
  bool background;
};

// method definitions
void print_cmd(command *cmd);
command *parse(vector<string> args);

#endif
