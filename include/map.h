#ifndef _MAP_H_
#define _MAP_H_

#include "game.h"

#define TILE_WALKABLE    BITS(0)
#define TILE_TRANSPARENT BITS(1)
#define TILE_VISIBLE     BITS(2)
#define TILE_OCCUPIED    BITS(3)

typedef struct tile_t {
	u16 flags;
	u8 symbol;
	u8 color;
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