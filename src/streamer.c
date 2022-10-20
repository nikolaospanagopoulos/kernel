#include "streamer.h"
#include "config.h"

struct diskStream *diskstreamerNew(int diskId) {
  struct disk *disk = getDisk(diskId);
  if (!disk) {
    return 0;
  }

  struct diskStream *streamer = kzalloc(sizeof(struct diskStream));
  streamer->pos = 0;
  streamer->disk = disk;
  return streamer;
}

int diskStreamerSeek(struct diskStream *stream, int pos) {
  stream->pos = pos;
  return 0;
}

int diskstreamerRead(struct diskStream *stream, void *out, size_t totalToRead) {
  int sector = stream->pos / SECTOR_SIZE;
  int offset = stream->pos % SECTOR_SIZE;
  char buff[SECTOR_SIZE];
  int res = diskReadBlock(stream->disk, sector, 1, buff);
  if (res < 0) {
    goto out;
  }
  size_t totalBytesToRead =
      totalToRead > SECTOR_SIZE ? SECTOR_SIZE : totalToRead;

  // always read 512 bytes
  for (int i = 0; i < totalBytesToRead; i++) {
    *(char *)out++ = buff[offset + i];
  }
  stream->pos += totalBytesToRead;
  if (totalToRead > SECTOR_SIZE) {
    res = diskstreamerRead(stream, out, totalToRead - SECTOR_SIZE);
  }

out:
  return res;
}

void diskStreamerFree(struct diskStream *stream) { kfree(stream); }
