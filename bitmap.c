#include <limits.h>
#include <stdint.h>

typedef uint32_t space_t;
enum { BITS_PER_SPACE = sizeof(space_t) * CHAR_BIT };
#define SPACE_OFFSET(b) ((b) / BITS_PER_SPACE)
#define BIT_OFFSET(b) ((b) % BITS_PER_SPACE)