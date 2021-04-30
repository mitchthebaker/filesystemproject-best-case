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
    struct bitmap_t* bitmap = malloc(sizeof(struct bitmap_t));

    //printf("size of uint8_t: %ld\n", sizeof(uint8_t));

    bitmap->volumeSize = volumeSize;
    bitmap->blockSize = blockSize;
    bitmap->numberOfBlocks = ceil(volumeSize / blockSize) - 1; // -1 to account for VCB
    //bitmap->freeSpaceBlocks = (bitmap->numberOfBlocks / bitmap->blockSize) + 1; // + 1 to account for decimal
    //bitmap->freespace = malloc((sizeof(uint8_t) * bitmap->numberOfBlocks)); // Malloc the freespace size according to the total number of blocks


    // May delete these if new implementations work
    bitmap->buf = malloc(MINBLOCKSIZE);
    bitmap->bufPosition = 0;
    bitmap->isBlockFree = malloc(bitmap->numberOfBlocks * sizeof(unsigned char));

    return bitmap;
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
            LBAwrite(aVCB_ptr, 1, 0);
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

// Initialize our map with initial values '1', which signifies that the blocks are free to use 
// Otherwise, blocks allocated with values of '0' signify that the blocks are allocated
int map_initialize(VCB * aVCB_ptr) {

    // Calculate the total number of freespace blocks
    uint64_t freeSpaceBlocks = (aVCB_ptr->numberOfBlocks / aVCB_ptr->sizeOfBlock) + 1; // +1 to account for rounding

    // Allocate memory for freespace
    uint8_t * freespace = malloc((sizeof(uint8_t) * aVCB_ptr->numberOfBlocks));

    // Iterate over the total number of blocks, and set each freespace block to 1
    // If the loop iterates past 'freeSpaceBlocks', then set these blocks to 0 since these are not for freespace
    for(int i = 0; i < aVCB_ptr->numberOfBlocks; i++) {

        if(i <= freeSpaceBlocks) {
            freespace[i] = 1;
        }
        else {
            freespace[i] = 0;
        }
    }

    // Initialize the LBA position of where the freespace begins
    aVCB_ptr->LBA_indexOf_freeSpace = 1;

    // Now allocate the freespace position into the LBA
    uint64_t blocksWritten = LBAwrite(freespace, freeSpaceBlocks, aVCB_ptr->LBA_indexOf_freeSpace);
    printf("number of freespace blocks written: %ld\n", blocksWritten);

    // Initialize index of freespace in VCB, then update VCB in LBA
    aVCB_ptr->LBA_indexOf_freeSpace = 1;
    aVCB_ptr->freeSpaceBlocks = freeSpaceBlocks;
    LBAwrite(aVCB_ptr, 1, 0);

    // Return the total number of freespace blocks written
    return blocksWritten;
}