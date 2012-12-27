static int move_player(void * ctx) {
	int k = *(int*)ctx;
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
	if (!BIT_ISSET(t->flags, TILE_WALKABLE) || BIT_ISSET(t->flags, TILE_OCCUPIED)) {
		game_d.player.pos.x -= m[v].x;
		game_d.player.pos.y -= m[v].y;
	}
	return 0;
}

static int toggle_view(void * ctx) {game_d.view++; return 0;}
static int toggle_fov_debug(void * ctx) {
	game_d.fov.mode = !game_d.fov.mode;
	game_d.fov.k = game_d.player.pos;
	game_d.fov.v = 0;
	return 0;
}
static int move_fov_debug  (void * ctx) {
	int k = *(int*)ctx;
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
static int nview_debug(void * ctx) {
	game_d.fov.v++;
	return 0;
}
