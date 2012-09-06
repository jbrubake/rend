#include "game.h"

#include "keymap.h"

static int move_player(uint k) {
	const static coord_t m[] = {
		{-1,-1},
		{ 0,-1},
		{ 1,-1},
		{-1, 0},
		{ 0, 0},
		{ 1, 0},
		{-1, 1},
		{ 0, 1},
		{ 1, 1},
	};
	uint v;
	switch (k)
	{
		case KEY_A1:    v = 0; break;
		case KEY_UP:
		case KEY_A2:    v = 1; break;
		case KEY_A3:    v = 2; break;
		case KEY_B1:
		case KEY_LEFT:  v = 3; break;
		case KEY_B2:    v = 4; break;
		case KEY_B3:
		case KEY_RIGHT: v = 5; break;
		case KEY_C1:    v = 6; break;
		case KEY_C2:
		case KEY_DOWN:  v = 7; break;
		case KEY_C3:    v = 8; break;
	}
	game_d.player.pos.x += m[v].x;
	game_d.player.pos.y += m[v].y;
	const tile_t* t = map_get_tile(game_d.map, game_d.player.pos.x, game_d.player.pos.y);
	if (!(t->flags & TILE_WALKABLE)) {
		game_d.player.pos.x -= m[v].x;
		game_d.player.pos.y -= m[v].y;
	}
	return 0;
}

static int toggle_view(uint k) {game_d.view++; return 0;}
static int toggle_fov_debug(uint k) {
	game_d.fov.mode = !game_d.fov.mode;
	game_d.fov.k = game_d.player.pos;
	game_d.fov.v = 0;
	return 0;
}
static int move_fov_debug  (uint k) {
	switch(k) {
		case 'a':
			game_d.fov.k.x--;
			break;
		case 'd':
			game_d.fov.k.x++;
			break;
		case 'w':
			game_d.fov.k.y--;
			break;
		case 's':
			game_d.fov.k.y++;
			break;
	}
	game_d.fov.v = 0;
	return 0;
}
static int nview_debug(uint k) {
	game_d.fov.v++;
	return 0;
}

static keyhashnode_t keynodes[] = {
	KEY_DEF(KEY_A1      , move_player),
	KEY_DEF(KEY_A2      , move_player),
	KEY_DEF(KEY_A3      , move_player),
	KEY_DEF(KEY_B1      , move_player),
	KEY_DEF(KEY_B2      , move_player),
	KEY_DEF(KEY_B3      , move_player),
	KEY_DEF(KEY_C1      , move_player),
	KEY_DEF(KEY_C2      , move_player),
	KEY_DEF(KEY_C3      , move_player),
	KEY_DEF(KEY_DOWN    , move_player),
	KEY_DEF(KEY_UP      , move_player),
	KEY_DEF(KEY_LEFT    , move_player),
	KEY_DEF(KEY_RIGHT   , move_player),
	KEY_DEF('+'         , toggle_view),
	KEY_DEF('k'         , toggle_fov_debug),
	KEY_DEF('a'         , move_fov_debug),
	KEY_DEF('d'         , move_fov_debug),
	KEY_DEF('w'         , move_fov_debug),
	KEY_DEF('s'         , move_fov_debug),
	KEY_DEF('z'         , nview_debug),
};

int game_init() {
	iface_init();
	uint i; for (i=0; i<sizeof(keynodes)/sizeof(*keynodes); i++) {
		key_add(keynodes + i);
	}
	game_d.map = map_init(80, 30);
	game_d.player.pos = (coord_t){1,1};
	return 0;
}

int game_loop() {
	while (1) {
		int c = iface_next_key();
		if (c == 'q') {break;}
		key_exec(c);
		fov_calc(game_d.map, game_d.player.pos, 6);
		iface_drawmap(game_d.map);
		mvprintw(30, 1, "                         ");
		mvprintw(30, 1, "%d %d", game_d.player.pos.x, game_d.player.pos.y);
		iface_swap();
	}
	return 0;
}

void game_clean() {
	iface_cleanup();
	map_clean(game_d.map);
}
