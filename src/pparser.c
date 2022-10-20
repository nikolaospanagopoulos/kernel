#include "pparser.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "status.h"
#include "string.h"

static int pathIsValid(const char *filename) {
  int length = strnlen(filename, MAX_PATH);
  return (length >= 3 && isDigit(filename[0]) &&
          memcmp((void *)&filename[1], ":/", 2) == 0);
}

static int getDiskNumberFromPath(const char **path) {
  if (!pathIsValid(*path)) {
    return -EBADPATH;
  }

  int driveNumber = toNumericDigit(*path[0]);

  *path += 3;
  return driveNumber;
}

static struct pathRoot *createRoot(int driveNumber) {
  struct pathRoot *root = kzalloc(sizeof(struct pathRoot));
  root->driveNumber = driveNumber;
  root->first = 0;
  return root;
}
static const char *getPathPart(const char **path) {

  char *resultPathPart = kzalloc(MAX_PATH);

  int i = 0;
  while (**path != '/' && **path != 0x00) {
    resultPathPart[i] = **path;
    *path += 1;
    i++;
  }
  if (**path == '/') {
    *path += 1;
  }

  if (i == 0) {
    kfree(resultPathPart);
    resultPathPart = 0;
  }

  return resultPathPart;
}

struct pathPart *parsePathPart(struct pathPart *last, const char **path) {
  const char *pathPartStr = getPathPart(path);
  if (!pathPartStr) {
    return 0;
  }

  struct pathPart *part = kzalloc(sizeof(struct pathPart));
  part->part = pathPartStr;
  part->next = 0x00;
  if (last) {
    last->next = part;
  }
  return part;
}

void freePath(struct pathRoot *root) {
  struct pathPart *part = root->first;
  while (part) {
    struct pathPart *nextPart = part->next;
    kfree((void *)part->part);
    kfree(part);
    part = nextPart;
  }

  kfree(root);
}
struct pathRoot *parse(const char *path, const char *currentDirectoryPath) {

  int res = 0;
  const char *tmpPath = path;

  struct pathRoot *pathRoot = 0;

  if (strlen(path) > MAX_PATH) {
    goto out;
  }
  res = getDiskNumberFromPath(&tmpPath);
  if (res < 0) {
    goto out;
  }
  pathRoot = createRoot(res);
  if (!pathRoot) {
    goto out;
  }
  struct pathPart *firstPart = parsePathPart(NULL, &tmpPath);
  if (!firstPart) {
    goto out;
  }
  pathRoot->first = firstPart;
  struct pathPart *part = parsePathPart(firstPart, &tmpPath);
  while (part) {
    part = parsePathPart(part, &tmpPath);
  }

out:
  return pathRoot;
}
