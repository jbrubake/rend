#include <limits.h>

#include "containers.h"
#include "game.h"

typedef struct line_t {
	coord_t s;
	coord_t e;
} line_t;

typedef struct view_t {
	line_t steep;
	line_t shallow;
} view_t;

static const view_t iview = { {{1, 0}, {0, 65535}}, {{0, 1}, {65535, 0}}};

static int fov_linepoint(line_t l, coord_t p) {
	coord_t v = l.e;
	p.x -= l.s.x;
	p.y -= l.s.y;
	v.x -= l.s.x;
	v.y -= l.s.y;
	v = (coord_t){-v.y, v.x};
	return p.x * v.x + p.y * v.y;
}

enum {
	ABOVE = 0,
	WITHIN = 1,
	BELOW = 2,
	STEEP_BUMP = 3,
	SHALLOW_BUMP = 4,
	BLOCKS = 5
};

static int fov_linesquare(line_t l, coord_t p) {
	int i[4];
	i[0] = fov_linepoint(l, p); p.x++;
	i[1] = fov_linepoint(l, p); p.y++;
	i[2] = fov_linepoint(l, p); p.x--;
	i[3] = fov_linepoint(l, p);
	// Note: >= and <= are intentional. Corners do not obstruct!
	if (i[0] >= 0 && i[1] >= 0 && i[2] >= 0 && i[3] >= 0) {return ABOVE;} // Above!
	if (i[0] <= 0 && i[1] <= 0 && i[2] <= 0 && i[3] <= 0) {return BELOW;} // Below!
	return WITHIN; // The line intersects the square.
}

static int fov_viewpoint(view_t* v, coord_t p) {
	int shallow = fov_linesquare(v->shallow, p);
	int steep   = fov_linesquare(v->steep  , p);
	switch (steep) {
		case ABOVE: return ABOVE;
		case WITHIN: return (shallow == WITHIN) ? BLOCKS : STEEP_BUMP;
		default:
			switch (shallow) {
				case ABOVE: return WITHIN;
				case WITHIN: return SHALLOW_BUMP;
				default: return BELOW;
			}
	}
}

void fov_split(view_t* vold, view_t*vnew, coord_t sq) {
	*vnew = *vold;

	vnew->shallow.e = sq;
	vold->steep.e   = sq;
}

void fov_calc(map_t* m, coord_t p, int range) {
	range*= 2; // Make sure to get the whole square
	int i,j;
	tile_t* t;

	view_t v[2];
	v[0] = iview;
	fov_split(v, v+1, (coord_t){5,5});

	for (i=0; i<m->size[0]; i++) {for (j=0; j<m->size[1]; j++) {
		t = map_get_tile(m, i, j);
		t->color = iface_color(COLOR_WHITE, COLOR_BLACK);
		BIT_UNSET(t->flags, TILE_VISIBLE);
	}}
	for     (i=0; i<range; i++) {
		for (j=0; j<range-i-1; j++) {
			t = map_get_tile(m, i+p.x, j+p.y);
			if (!t) {continue;}
			coord_t x = {i, j};
			const static u8 col [] =
				{iface_color(COLOR_RED,    COLOR_BLACK),
				 iface_color(COLOR_GREEN,  COLOR_BLACK),
				 iface_color(COLOR_BLUE,   COLOR_BLACK),
				 iface_color(COLOR_RED,    COLOR_GREEN),
				 iface_color(COLOR_BLUE,   COLOR_GREEN),
				 iface_color(COLOR_BLACK,  COLOR_WHITE),};

			t->color = col[fov_viewpoint(v + ((game_d.view%2) ? 0 : 1), x)];
			BIT_SET(t->flags, TILE_VISIBLE);
		}
	}
}
