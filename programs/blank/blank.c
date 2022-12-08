#include "os.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
int main(int argc, char **argv) {

  printf("blank");

  char str[80] = "This is - www.tutorialspoint.com - website";
  char *s = "-";
  char *token;

  /* get the first token */
  token = strtok(str, s);

  /* walk through other tokens */
  while (token != NULL) {
    printf(" %s\n", token);

    token = strtok(NULL, s);
  }

  free(token);
  while (1) {
  }
  return 0;
}
