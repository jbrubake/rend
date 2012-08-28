#ifndef _MAP_H_
#define _MAP_H_

#include "types.h"

#define BITS(x) (1<<(x))
#define TILE_WALKABLE    BITS(0)
#define TILE_TRANSPARENT BITS(1)

#define COL_BLUE    BITS(0)
#define COL_GREEN   BITS(1)
#define COL_RED     BITS(2)
#define COL_WHITE   COL_BLUE | COL_GREEN | COL_RED
#define COL_INTENSE BITS(3)

#define COL_FG(x)   (x)
#define COL_BG(x)   (x<<4)

typedef struct tile_t {
	u16 flags;
	u8 symbol;
	u8 colour;
} tile_t;

typedef struct map_t {
	ushort size[2];
	tile_t tiles[0];
} map_t;

// Use this when statically allocating space for a map_t
map_t * map_init (ushort sizex, ushort sizey);
void    map_clean(map_t *m);
tile_t* map_get_tile(map_t* w, int x, int y);

#endif