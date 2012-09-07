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
	llist_t steepbumps;
	llist_t shallowbumps;
} view_t;

static const view_t iview = {
	{{1, 0}, {0, 65535}},        // Steep line
	{{0, 1}, {65535, 0}},        // Shallow line
	llist_init(sizeof(coord_t)), // Bump list
	llist_init(sizeof(coord_t))  // Bump list
};

static void fov_destroy(view_t* x) {
		llist_clean(&x->shallowbumps);
		llist_clean(&x->steepbumps);
}

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
	BLOCKED = 5
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

static int fov_doescontain(line_t l, coord_t p) {return !fov_linepoint(l, p);}

// Checks for and culls any unncessary views.
static void fov_cullview(llist_node_t **hvn)
{
	view_t * const f = (view_t*)((*hvn)->data);

	if (   fov_doescontain(f->shallow, f->steep.s)
		&& fov_doescontain(f->shallow, f->steep.e)
		&& (   fov_doescontain(f->shallow, (coord_t){0, 1})
		    || fov_doescontain(f->shallow, (coord_t){1, 0}))  )
	{
		fov_destroy((view_t*)(*hvn)->data);
		llist_remove(hvn);
	}
}

static int fov_viewpoint(view_t* v, coord_t p) {
	int shallow = fov_linesquare(v->shallow, p);
	int steep   = fov_linesquare(v->steep  , p);
	switch (steep) {
		case ABOVE: return ABOVE;
		case WITHIN: return (shallow == WITHIN) ? BLOCKED : STEEP_BUMP;
		default:
			switch (shallow) {
				case ABOVE: return WITHIN;
				case WITHIN: return SHALLOW_BUMP;
				default: return BELOW;
			}
	}
}

static void fov_copylist(llist_t *lnew, llist_t *lold) {
	*lnew = llist_init(sizeof(view_t));
	llist_node_t* i = lold->f;
	while (i) {llist_add(lnew, i->data); i = i->n;}
}

static void fov_shallowbump(llist_node_t** hln, coord_t b) {
	view_t * const v = (void*)(*hln)->data;
	v->shallow.e = (coord_t){b.x, b.y+1};

	llist_add(&v->shallowbumps, &b);

	llist_node_t * bumps = v->steepbumps.f;
	int r;
	while (bumps) {
		coord_t * const b2 = (void*)bumps->data;
		r = fov_linesquare(v->shallow, *b2);
		switch(r) {
			case WITHIN:
			case BELOW:
				v->shallow.s = (coord_t){b2->x+1, b2->y};
			default: break;
		}
		bumps = bumps->n;
	}
	fov_cullview(hln);
}

static void fov_steepbump(llist_node_t** hln, coord_t b) {
	view_t * const v = (void*)(*hln)->data;
	v->steep.e = (coord_t){b.x+1, b.y};

	llist_add(&v->steepbumps, &b);

	llist_node_t * bumps = v->shallowbumps.f;
	int r;

	while (bumps) {
		coord_t * const b2 = (void*)bumps->data;
		r = fov_linesquare(v->steep, *b2);
		switch(r) {
			case WITHIN:
			case ABOVE:
				{static int i = 0;
				v->steep.s = (coord_t){b2->x, b2->y+1};
				 if (!i) {mvprintw(35, 1, "HIT: %d %d %d", r, v->steep.s.x, v->steep.s.y); i++;}}
			default: break;
		}
		bumps = bumps->n;
	}
	fov_cullview(hln);
}

static void fov_split(llist_node_t **hln, coord_t sq) {
	view_t * const vold = (void*)(*hln)->data;
	view_t * const vnew = (void*)llist_addafter(*hln, vold)->data;

	fov_copylist(&vnew->shallowbumps, &vold->shallowbumps);
	fov_copylist(&vnew->steepbumps,   &vold->steepbumps);

	{llist_node_t * n = (*hln)->n;
	 fov_shallowbump(&n,  sq);}
	fov_steepbump  (hln, sq);
}

static void fov_clear(map_t* m) {
	uint i; for (i=0; i<m->size[0] * m->size[1]; i++) {
		BIT_UNSET(m->tiles[i].flags, TILE_VISIBLE);
		m->tiles[i].color = iface_color(COLOR_WHITE, COLOR_BLACK);
	}
}

static void fov_visit(tile_t* t, llist_node_t** cview, coord_t x) {
	uint tiledone = 0;
	while ((*cview) && !tiledone) {
		switch (fov_viewpoint((view_t*)(*cview)->data, x)) {
			case ABOVE: // All remaining views are unnecessary for this diagonal.
				(*cview) = (*cview)->n;
				continue;
			case BELOW: // This view is stale. Try the next fresh one.
				tiledone = 1; break;
			case WITHIN: // The tile is visible, and may split the view.
				BIT_SET(t->flags, TILE_VISIBLE);
				if (!(t->flags & TILE_TRANSPARENT)) {fov_split(cview, x);}
				tiledone = 1; break;
			case SHALLOW_BUMP:
				BIT_SET(t->flags, TILE_VISIBLE);
				if (!(t->flags & TILE_TRANSPARENT)) {fov_shallowbump(cview, x);}
				tiledone = 1; break;
			case STEEP_BUMP:
				BIT_SET(t->flags, TILE_VISIBLE);
				if (!(t->flags & TILE_TRANSPARENT)) {fov_steepbump  (cview, x);}
				tiledone = 1; break;
			case BLOCKED:
				BIT_SET(t->flags, TILE_VISIBLE);
				if (!(t->flags & TILE_TRANSPARENT)) {
					fov_destroy((view_t*)(*cview)->data);
					llist_remove(cview);
				}
				tiledone = 1; break;
		}
	}
}

static void fov_calc_debug(map_t* m, coord_t p, llist_t* views, int xmult, int ymult) {
	int i, j; tile_t *t; int r;
	llist_node_t* c = views->f;
	for (i=0; i<game_d.fov.v && c; i++) {c = c->n;}
	if (!c) {game_d.fov.v = 0; c = views->f;}
	if (!c) {return;}
	view_t* v = (view_t*)(c->data);

	for (i=0; i<m->size[0]*m->size[1]; i++) {
		m->tiles[i].color = iface_color(COLOR_WHITE, COLOR_BLACK);
		BIT_SET(m->tiles[i].flags, TILE_VISIBLE);
	}

	const u8 col[] = {
		iface_color(COLOR_RED,   COLOR_BLACK),
		iface_color(COLOR_GREEN, COLOR_BLACK),
		iface_color(COLOR_BLUE,  COLOR_BLACK),
		iface_color(COLOR_RED,   COLOR_WHITE),
		iface_color(COLOR_BLUE,  COLOR_WHITE),
	};

	for (i=0; i<m->size[0]; i++) {for (j=0; j<m->size[1]; j++) {
		coord_t x = {(i-j), j};
		coord_t l = {x.x * xmult, x.y * ymult};
		r = fov_viewpoint(v, x);
		t = map_get_tile(m, l.x + p.x, l.y + p.y);
		if (!t) {continue;}
		t->color = col[r];
	}}
	mvprintw(31, 1, "                                             ");
	mvprintw(32, 1, "                                             ");
	mvprintw(31, 1, "Shallow: %d %d -> %d %d", v->shallow.s.x, v->shallow.s.y, v->shallow.e.x, v->shallow.e.y);
	mvprintw(32, 1, "Steep: %d %d -> %d %d"  , v->steep.s.x, v->steep.s.y, v->steep.e.x, v->steep.e.y);
}

static void fov_calc_quarter(map_t* m, coord_t p, int range, int xmult, int ymult) {
	range*= 2; // Make sure to get the whole square
	int i,j, skip;
	tile_t* t;

	llist_t views = llist_init(sizeof(view_t));
	llist_node_t *cview;
	llist_add(&views, &iview);

	cview = views.f;

	for     (i=0;                          i<range;                       i++) {
		for (j=0, cview = views.f, skip=0; cview && j<(i+1) && !skip; j++) {
			coord_t x = {(i-j), j};                 // Coordinates for the purpose of calculations.
			coord_t l = {x.x * xmult, x.y * ymult}; // Coordinates for the purpose of map lookups.
			t = map_get_tile(m, l.x+p.x, l.y+p.y);
			if (!t) {continue;}
			fov_visit(t, &cview, x);
			if (game_d.fov.mode && game_d.fov.k.x == l.x + p.x && game_d.fov.k.y == l.y + p.y) {fov_calc_debug(m, p, &views, xmult, ymult); i = 10000; j = 100000; continue;}
		}
	}
	cview = views.f;
	while (cview) {
		fov_destroy((view_t*)cview->data);
		llist_remove(&cview);
	}
}

void fov_calc (map_t* m, coord_t p, int range) {
	fov_clear(m);
	fov_calc_quarter(m, p, range, -1,  1);
	fov_calc_quarter(m, p, range, -1, -1);
	fov_calc_quarter(m, p, range,  1, -1);
	fov_calc_quarter(m, p, range,  1,  1);
}
