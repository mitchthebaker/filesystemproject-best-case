#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "fsVCB.h"
#include "bitmap.h"
#include "fsLow.h"

#define CHAR_OFFSET(b) ((b/CHAR_BIT))
#define BIT_OFFSET(b) ((b%CHAR_BIT))

struct bitmap_t* create_bitmap(int volumeSize, int blockSize){
    struct bitmap_t* p = malloc(sizeof(struct bitmap_t));

    p->volumeSize = volumeSize;
    p->blockSize = blockSize;
    p->numberOfBlocks = ceil(volumeSize / blockSize) - 1; // -1 to account for VCB
    p->buf = malloc(MINBLOCKSIZE);
    p->bufPosition = 0;
    p->isBlockFree = malloc(p->numberOfBlocks * sizeof(unsigned char));

    return p;
}

int map_getBit(unsigned char* c, int pos){
    return ((*c >> pos) & 1);
}

void map_setBit(unsigned char* c, int bit, int pos){

    *c ^= (-bit ^ *c) & (1 << pos);
}

int map_set(struct bitmap_t* bitmap, int val, int i){

    if (i > MAX_BITS)
        return -1;

    map_setBit(&bitmap->isBlockFree[CHAR_OFFSET(i)], val, BIT_OFFSET(i));

    return 0;
}

int map_get(struct bitmap_t* bitmap, int i){
    if (i > MAX_BITS)
        return -1;

    return map_getBit(&bitmap->isBlockFree[CHAR_OFFSET(i)],BIT_OFFSET(i));
}

/* 5 blocks total for storing freespace on disk 
 * for i = 0 to 5 (total blocks):
 *
 *  allocate temp buf for holding 1 512 byte blob of data 
 *  char * buf = malloc(MINBLOCKSIZE);
 *
 *  now for each blocks iterate over each bit (512*8=4096)
 *  for j = 0 to 4096 bits:
 */ 
void map_init(VCB * aVCB_ptr, struct bitmap_t* bitmap, uint64_t sizeInBlocks, uint64_t totalBytes) {

    // Initialize iterator; multiply blocksize by 8 to get total bits in one block
    int i, bitsPerBlock = bitmap->blockSize * 8;

    // Iterate over total number of blocks required for storing freespace
    for (i = 0; i < sizeInBlocks; i++) {
        
        // Iterate over each bit in a single block instance; allocate with 'mapinit_set()'
        int j;
        for(j = 0; j < bitsPerBlock; j++) {

            // Update bytes in 'bitmap->buf', which we will then use to LBAwrite() with 
            mapinit_set(bitmap, 1, j);
        }

        // Output buffer for testing
        // printf("\nbuf: %s\n", bitmap->buf);

        // Write one block to the LBA
        LBAwrite(bitmap->buf, 1, i + 1);

        // Signifies first block of freespace; set index to 'i + 1' since VCB is at index 0
        if(i == 0) {
            aVCB_ptr->LBA_indexOf_freeSpace = i + 1;
        }
    }
}

void mapinit_setBit(struct bitmap_t* bitmap, unsigned char* c, int bit, int pos) {

    //printf("bitValue: %d, bitPositionInByte: %d\n", bit, pos);

    *c ^= (-bit ^ *c) & (1 << pos);

    unsigned char * bytes = malloc(1 * sizeof(unsigned char));
    unsigned char * bits = malloc(8 * sizeof(unsigned char));

    int i;
    for(i = 0; i < 8; i++) {
        bits[i] = ((1 << (i % 8)) & (bytes[i/8])) >> (i % 8);
    }
    
    int j;
    for(j = 0; j < 8; j++) {
        //bits[k] ^= (-bit ^ *c) & (1 << pos);
        bits[j] = bit;

        // output to console the updated 8 bits
        //printf("%d", bits[j]);
    }
    //printf("\n---------\n");
    
    memcpy(bitmap->buf + bitmap->bufPosition, bytes, 8);
    bitmap->bufPosition = (bitmap->bufPosition <= MINBLOCKSIZE) ? bitmap->bufPosition + 1 : 0;
    //printf("bufPosition: %ld\n", bitmap->bufPosition);
    //printf("buf: %s\n", buf);

    // corresponding free for 'bytes'/'bits' malloc
    free(bytes);
    free(bits);
}

int mapinit_set(struct bitmap_t* bitmap, int val, int i) {

    if (i > MAX_BITS)
        return -1;

    mapinit_setBit(bitmap, &bitmap->isBlockFree[CHAR_OFFSET(i)], val, BIT_OFFSET(i));

    return 0;
}