// Ben Harris

#ifndef _PARSE_H_
#define _PARSE_H_

#include <string>
#include <vector>
using namespace std;

// command struct
struct command {
  char** args;
  bool background;
  string piping;
  string infile;
  string outfile;
};

// method definitions
void print_cmd(command *cmd);
command *parse(vector<string> args);

#endif
