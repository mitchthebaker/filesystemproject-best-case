#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "bitmap.h"

#define CHAR_OFFSET(b) ((b/CHAR_BIT))
#define BIT_OFFSET(b) ((b%CHAR_BIT))

struct bitmap_t* create_bitmap(int volumeSize, int blockSize){
    struct bitmap_t* p = malloc(sizeof(struct bitmap_t));

    p->volumeSize = volumeSize;
    p->blockSize = blockSize;
    p->numberOfBlocks = ceil(volumeSize / blockSize);

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

void map_init(struct bitmap_t* bitmap){
    int i;
    for (i = 0; i<MAX_BITS; i++){
        map_set(bitmap,0,i);
    }
}