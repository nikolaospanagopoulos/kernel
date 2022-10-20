#pragma once
#include "disk.h"
#include "kernelHeap.h"

struct diskStream {

  int pos;
  struct disk *disk;
};
struct diskStream *diskstreamerNew(int diskId);
int diskStreamerSeek(struct diskStream *stream, int pos);
int diskstreamerRead(struct diskStream *stream, void *out, size_t totalToRead);
void diskStreamerFree(struct diskStream *stream);
