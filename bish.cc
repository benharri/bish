#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <unistd.h>
using namespace std;

char** parse_line(char* line) {
  char** cmd_w_args;
  int cnt = 0;
  char *pch = strtok (line," ");
  while (pch != NULL) {
    cmd_w_args[cnt] = pch;
    pch = strtok (NULL, " "); cnt++;
  }
  return cmd_w_args;
}

int main(int argc, char **argv){
  string line;

  cout << "bish$ ";
  while (getline(cin, line)) {
    cout << line << endl;
    execv(line, parse_line(line.c_str()), NULL);
    cout << "bish$ ";
  }
  return 0;
}
