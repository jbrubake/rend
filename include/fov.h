#include "game.h"

void fov_calc(map_t* m, coord_t p) {
	uint i,j;
	tile_t* t;
	for (i=0; i<m->size[0] * m->size[1]; i++) {
		BIT_UNSET(m->tiles[i].flags, TILE_VISIBLE); // Unset visibility flag everywhere
	}
	for (j=0; j<12; j++) {for (i=0; i<12-j-1; i++) {
		t = map_get_tile(m, i + p.x, j + p.y);
		if (t) {BIT_SET(t->flags, TILE_VISIBLE);}
	}}
}
