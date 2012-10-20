#ifndef _GAME_H_
#define _GAME_H_

typedef struct coord_t {int x; int y;} coord_t;

#include "containers.h"
#include "types.h"
#include "map.h"
#include "interface.h"
#include "fov.h"
#include "kiss.h"
#include "creature.h"

#include "debug.h"

#define BITS(x) (1<<(x))
#define BIT_SET(x, y)    {x |= y;}
#define BIT_UNSET(x, y)  {x &= ~(y);}
#define BIT_TOGGLE(x, y) {x ^= y;}
#define BIT_ISSET(x, y)  (!!((x) & (y))) // The infamous cast-to-bool operator

typedef struct event_t {
	ref_t refc;
	int type;
	int priority;
} event_t;

typedef struct event_rest_t {
	event_t event;
	creature_t *creature;
} event_rest_t;

// Global game data
struct game_t {
	int time;
	uint view;
	struct {
		uint mode;
		coord_t k;
		uint v;
	} fov;
	struct {
		// This is where the "debug" variables and such go.
		template_t humanoid;
	} transient;
	heap_t  pqueue;
	reflist_t goblins;
	creature_t player;
	map_t *map;
	iface_t iface;
} game_d;

int  game_init();
int  game_loop();
void game_clean();

#endif
