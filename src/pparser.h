#pragma once

struct pathRoot {
  int driveNumber;
  struct pathPart *first;
};

struct pathPart {

  const char *part;
  struct pathPart *next;
};
