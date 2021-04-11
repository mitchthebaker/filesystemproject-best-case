#include <limits.h>
#include <stdint.h>
#include "bitmap.h"
typedef uint32_t space_t;
enum { BITS_PER_SPACE = sizeof(space_t) * CHAR_BIT };
#define SPACE_OFFSET(b) ((b) / BITS_PER_SPACE)
#define BIT_OFFSET(b) ((b) % BITS_PER_SPACE)

//give up today, will rewrite tomorrow
void bitmap_init(struct bitmap_t* bitmap);
int bitmap_set(struct bitmap_t* bitmap, int value, int index);
int bitmap_get(struct bitmap_t* bitmap, int index);



void set_bit(space_t *words, int n) { 
    words[SPACE_OFFSET(n)] |= ((space_t) 1 << BIT_OFFSET(n));
}

void clear_bit(space_t *words, int n) {
    words[SPACE_OFFSET(n)] &= ~((space_t) 1 << BIT_OFFSET(n)); 
}

int get_bit(space_t *words, int n) {
    space_t bit = words[SPACE_OFFSET(n)] & ((space_t) 1 << BIT_OFFSET(n));
    return bit != 0; 
}

