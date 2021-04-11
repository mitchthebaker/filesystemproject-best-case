#ifndef BITMAP_H
#define BITMAP_H

#define bitSize (CHAR_BIT*sizeof(byte))

int get_bit   (space_t *, int);
void set_bit   (space_t *, int);
void clear_bit (space_t *, int);

typedef struct {
    int num_bits;
    char* entries;
}bitmap_t;

void bitmap_init(struct bitmap_t* bitmap);
int bitmap_set(struct bitmap_t* bitmap, int value, int index);
int bitmap_get(struct bitmap_t* bitmap, int index);

#endif