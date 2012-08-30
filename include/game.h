#ifndef _GAME_H_
#define _GAME_H_

#include "types.h"
#include "map.h"
#include "interface.h"

#include "debug.h"

#define BITS(x) (1<<(x))
#define BIT_SET(x, y)    {x |= y;}
#define BIT_UNSET(x, y)  {x &= ~(y);}
#define BIT_TOGGLE(x, y) {x ^= y;}
#define BIT_ISSET(x, y)  (!!((x) & (y))) // The infamous cast-to-bool operator

typedef struct coord_t {uint x; uint y;} coord_t;

typedef struct actor_t {
	uint symbol;
	uint attrib;
	coord_t pos;
} actor_t;

// Global game data
struct game_t {
	uint time;
	actor_t player;
	map_t *map;
} game_d;

int  game_init();
int  game_loop();
void game_clean();

#endif
