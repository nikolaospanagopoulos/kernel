#pragma once

struct pathRoot {
  int driveNumber;
  struct pathPart *first;
};

struct pathPart {

  const char *part;
  struct pathPart *next;
};

struct pathRoot *parse(const char *path, const char *currentDirectoryPath);
void freePath(struct pathRoot *root);
