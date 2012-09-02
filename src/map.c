#include <stdlib.h>
#include <string.h>

#include "game.h"

#define CLAMP(x, u, v) ((x<(u) && x<(v)) || (x>(u) && x>(v)))
#define map_getsize(tiles) ((tiles) * sizeof(tile_t) + sizeof(map_t))

map_t * map_init (ushort sizex, ushort sizey) {
	map_t *m;
	tile_t* t;
	uint osize = sizex * sizey;
	m = malloc(map_getsize(osize));
	m->size[0] = sizex; m->size[1] = sizey;
	int i, j;
	const tile_t wall = {0,                                '#', iface_color(COLOR_WHITE, COLOR_BLACK)};//iface_color(COLOR_CYAN,  COLOR_BLACK)};
	const tile_t flr  = {TILE_WALKABLE | TILE_TRANSPARENT, '.', iface_color(COLOR_WHITE, COLOR_BLACK)};//iface_color(COLOR_WHITE, COLOR_BLACK)};
	const tile_t plr  = {0,                                '#', iface_color(COLOR_WHITE, COLOR_BLACK)};//iface_color(COLOR_RED  , COLOR_BLACK)};
	for (i=0; i<sizex; i++) {
		for (j=0; j<sizey; j++) {
			t = map_get_tile(m, i, j);
			if (i == 0 || i == sizex-1 || j == 0 || j == sizey-1) {*t =  wall; continue;}
			*t = (!(i % 5) && !(j % 5)) ? plr : flr;
		}
	}
	return m;
}
void    map_clean(map_t *m) {free(m);}

tile_t* map_get_tile(map_t* w, int x, int y) {
	if (CLAMP(x, 0, w->size[0]-1) || CLAMP(y, 0, w->size[1]-1)) {return NULL;}
	return w->tiles + x + w->size[0]*y;
}
