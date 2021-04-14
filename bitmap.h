#ifndef BITMAP_H
#define BITMAP_H
#include <limits.h>
//not sure about the size, so let's change it at meeting
#define MAX_CHARS 128
#define MAX_BITS MAX_CHARS*CHAR_BIT

struct bitmap_t {
  uint64_t volumeSize;
  uint64_t blockSize;
  uint64_t numberOfBlocks;
  unsigned char *isBlockFree;
};

struct bitmap_t* create_bitmap(int, int);

// VOLUME_SIZE = 10000384 bytes
// BLOCK_SIZE = 512 bytes
// NUMBER_OF_BLOCKS = VOLUME_SIZE / BLOCK_SIZE
// BIT_ARRAY[NUMBER_OF_BLOCKS]

// NUMBER_OF_BLOCKS bits / 8 bits (1 byte) = 2442 bytes
// BITMAP_SIZE = 2442 bytes
// BITMAP_BY_BLOCKS = 5 blocks DISK SPACE




int map_getBit(unsigned char* c, int pos);
void map_setBit(unsigned char* c, int bit, int pos);

int map_set(struct bitmap_t* bitmap, int val, int index);
int map_get(struct bitmap_t* bitmap, int index);
void map_init(struct bitmap_t* bitmap);

#endif