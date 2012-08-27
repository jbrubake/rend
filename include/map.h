#ifndef _MAP_H_
#define _MAP_H_

#include "types.h"

#define CLAMP(x, u, v) (x<(u) && x<(v) || x>(u) && x>(v))
#define BITS(x) (1<<(x))
#define FLAG_WALKABLE = BITS(1)

typedef struct tile_t {
	uint flags;
	char symbol;
} tile_t;

typedef struct map_t {
	ushort size[2];
	tile_t tiles[0];
} map_t;

tile_t* map_get_tile(map_t* w, int x, int y) {
	if (CLAMP(x, 0, w->size[0]) || CLAMP(y, 0, w->size[1])) {return NULL;}
	return w->tiles[x + w->size[0]*y];
}

#endif