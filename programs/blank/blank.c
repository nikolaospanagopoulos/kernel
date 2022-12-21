#include "os.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
int main(int argc, char **argv) {

  char *myStr = (char *)malloc(sizeof("Vue is terrible"));

  strncpy(myStr, "Vue is terrible", sizeof("Vue is terrible"));

  printf("%s", myStr);

  free(myStr);

  return 0;
}
