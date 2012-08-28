#include "map.h"
#include <stdlib.h>
#include <string.h>

#define CLAMP(x, u, v) ((x<(u) && x<(v)) || (x>(u) && x>(v)))
#define map_getsize(tiles) ((tiles) * sizeof(tile_t) + sizeof(map_t))

map_t * map_init (ushort sizex, ushort sizey) {
	map_t *m;
	tile_t* t;
	uint osize = sizex * sizey;
	m = malloc(map_getsize(osize));
	m->size[0] = sizex; m->size[1] = sizey;
	int i, j;
	const tile_t wall = {0,                                '#', COL_FG(COL_BLUE | COL_GREEN)};
	const tile_t flr  = {TILE_WALKABLE | TILE_TRANSPARENT, ' ', COL_FG(COL_WHITE | COL_INTENSE)};
	for (i=0; i<sizex; i++) {
		for (j=0; j<sizey; j++) {
			t = map_get_tile(m, i, j);
			*t = (i == 0 || i == sizex-1 || j == 0 || j == sizey-1) ? wall : flr;
		}
	}
	return m;
}
void    map_clean(map_t *m) {free(m);}

tile_t* map_get_tile(map_t* w, int x, int y) {
	if (CLAMP(x, 0, w->size[0]) || CLAMP(y, 0, w->size[1])) {return NULL;}
	return w->tiles + x + w->size[0]*y;
}
