#ifndef _GAME_H_
#define _GAME_H_

typedef struct coord_t {int x; int y;} coord_t;

#include "containers.h"
#include "types.h"
#include "map.h"
#include "interface.h"
#include "fov.h"
#include "kiss.h"

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

typedef struct actor_t {
	ref_t refc;
	coord_t pos;
	u8 symbol;
	u8 color;
} actor_t;

typedef struct event_rest_t {
	event_t event;
	actor_t *creature;
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
	heap_t  pqueue;
	reflist_t goblins;
	actor_t player;
	map_t *map;
} game_d;

int  game_init();
int  game_loop();
void game_clean();

#endif
