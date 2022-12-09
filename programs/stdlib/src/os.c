#include "os.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct commandArgument *osParseCommand(const char *command, int max) {

  struct commandArgument *rootCommand = 0;
  char scommand[1024];
  if (max >= (int)sizeof(scommand)) {
    return 0;
  }
  strncpy(scommand, command, sizeof(scommand));
  char *token = strtok(scommand, " ");

  if (!token) {
    goto out;
  }
  rootCommand = osMalloc(sizeof(struct commandArgument));
  if (!rootCommand) {
    goto out;
  }
  strncpy(rootCommand->argument, token, sizeof(rootCommand->argument));
  rootCommand->next = 0;

  struct commandArgument *current = rootCommand;
  token = strtok(NULL, " ");

  while (token != 0) {
    struct commandArgument *newCommand =
        osMalloc(sizeof(struct commandArgument));
    if (!newCommand) {
      break;
    }
    strncpy(newCommand->argument, token, sizeof(newCommand->argument));
    newCommand->next = 0x00;
    current->next = newCommand;
    current = newCommand;
    token = strtok(NULL, " ");
  }

out:
  osFree(token);
  return rootCommand;
}
int osGetKeyBlock() {
  int val = 0;

  do {

    val = osGetKey();

  } while (val == 0);
  return val;
}
void osTerminalReadLine(char *out, int max, bool outputWhileTyping) {
  int i = 0;
  for (i = 0; i < max - 1; i++) {
    char key = osGetKeyBlock();
    if (key == 13) {
      break;
    }

    if (outputWhileTyping) {
      putchar(key);
    }
    if (key == 0x08 && i >= 1) {
      out[i - 1] = 0x00;
      i -= 2;
      continue;
    }
    out[i] = key;
  }
  out[i] = 0x00;
}
