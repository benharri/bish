#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

int main(int argc, char **argv){
  string line;
  cout << getenv("PS1");
  while (getline(cin, line)) {
    cout << line << endl;
    cout << getenv("PS1");
  }
  return 0;
}
