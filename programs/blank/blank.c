#include "../stdlib/src/os.h"
#include "../stdlib/src/stdlib.h"
int main(int argc, char **argv) {

  print("nikos");

  void *ptr = malloc(512);

  while (1) {
    if (getKey() != 0) {
      print("key was pressed");
    }
  }

  return 0;
}
