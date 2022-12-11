#include "shell.h"
#include "os.h"
#include "stdio.h"
#include "stdlib.h"
int main(int argc, char **argv) {

  print("PanagopoulOS v0.0.1\n");

  while (1) {
    print("> ");
    char buff[1024];
    osTerminalReadLine(buff, sizeof(buff), true);
    print("\n");
    osSystemRun(buff);
    print("\n");
  }

  return 0;
}
