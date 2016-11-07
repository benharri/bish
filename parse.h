// Ben Harris

#ifndef _PARSE_
#define _PARSE_

#include <string>
#include <vector>
#include <iostream>
using namespace std;

// method definitions
// command struct
struct command {
  char*** args;
  bool background;
  string piping;
  string infile;
  string outfile;
};

void print_cmd(command *cmd);
command *parse(vector<string> args);

#endif
