#include "os.h"

extern int main(int argc, char **argv);

void cStart() {
  struct processArguments arguments;
  osProcessGetArguments(&arguments);
  int res = main(arguments.argc, arguments.argv);
  if (res == 0) {
  }
}
