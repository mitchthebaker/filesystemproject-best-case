#ifndef BITMAP_H
#define BITMAP_H

#include <limits.h>
//not sure about the size, so let's change it at meeting
#define MAX_CHARS 128
#define MAX_BITS MAX_CHARS*CHAR_BIT

struct bitmap_t {
    unsigned char array[MAX_CHARS];
};

int map_getBit(unsigned char* c, int pos);
void map_setBit(unsigned char* c, int bit, int pos);

int map_set(struct bitmap_t* bitmap, int val, int index);
int map_get(struct bitmap_t* bitmap, int index);
void map_init(struct bitmap_t* bitmap);

#endif