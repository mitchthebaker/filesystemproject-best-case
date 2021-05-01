#include <limits.h>
#include "fsVCB.h"

#ifndef BITMAP_H
#define BITMAP_H

int map_initialize(VCB * aVCB_ptr);

uint64_t allocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToAlloc, uint64_t blockPos);

uint64_t deallocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToDealloc, uint64_t blockPos);

uint64_t requestFSBlocks(VCB * aVCB_ptr, uint64_t numFSBlocksRequested);

#endif

/*#define MAX_CHARS 128
#define MAX_BITS MAX_CHARS*CHAR_BIT

struct bitmap_t {
    uint64_t volumeSize;
    uint64_t blockSize;
    uint64_t numberOfBlocks;
    uint64_t freeSpaceBlocks;
    uint8_t  freespace;

    unsigned char *isBlockFree;
    char * buf;
    uint64_t bufPosition;
};

struct bitmap_t* create_bitmap(int, int);

// VOLUME_SIZE = 10000000 bytes
// BLOCK_SIZE = 512 bytes
// NUMBER_OF_BLOCKS = VOLUME_SIZE / BLOCK_SIZE = 19531 

// BIT_ARRAY[NUMBER_OF_BLOCKS]

// NUMBER_OF_BLOCKS bits / 8 bits (1 byte) = 2442 bytes
// BITMAP_SIZE = 2442 bytes
// BITMAP_BY_BLOCKS = 5 blocks DISK SPACE

int map_getBit(unsigned char* c, int pos);
void map_setBit(unsigned char* c, int bit, int pos);
int map_set(struct bitmap_t* bitmap, int val, int index);
int map_get(struct bitmap_t* bitmap, int index);
void map_init(VCB * aVCB_ptr, struct bitmap_t* bitmap, uint64_t size, uint64_t totalBytes);

void mapinit_setBit(struct bitmap_t* bitmap, unsigned char* c, int bit, int pos);
int mapinit_set(struct bitmap_t* bitmap, int val, int index);
*/