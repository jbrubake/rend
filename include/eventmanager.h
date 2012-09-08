static const coord_t where[] = {
	{ 0, -1},
	{ 0, +1},
	{-1,  0},
	{+1,  0},
	{+1, -1},
	{+1, +1},
	{-1, -1},
	{-1, +1}
};
static int gobbo_move(event_t* ev) {
	actor_t * const g = *(actor_t* *)ev->data;
	const coord_t * const w = where + kiss_rand()%8;
	tile_t* t = map_get_tile(game_d.map, g->pos.x + w->x, g->pos.y + w->y);
	if ( t && !BIT_ISSET(t->flags, TILE_OCCUPIED) && BIT_ISSET(t->flags, TILE_WALKABLE) ) {
		g->pos.x += w->x;
		g->pos.y += w->y;
	}

	ref_copy(ev); // Make sure the event is not deleted.
	ev->priority = game_d.time + 100; // Renew
	heap_push(&game_d.pqueue, ev);
	return 0;
}
static int player_move(event_t* ev) {
	int c;

	while (1) {
		c = iface_next_key();
		if (c == 'q') {return 1;}
		if (key_exec(c) > 0) {continue;}
		break;
	}

	ref_copy(ev); // Make sure the event is not deleted.
	ev->priority = game_d.time + 25; // Renew
	heap_push(&game_d.pqueue, ev);
	return 0;
}

typedef int (*event_func)(event_t*);
enum {
	GOBBO_REST,
	PLAYER_REST,
};
static const event_func event_funcs[] = {
	gobbo_move,  // GOBBO_REST
	player_move, // PLAYER_REST
};
static int handle_event(event_t* ev) {
	event_func evf = event_funcs[ev->type];
	int r = evf(ev);
	ref_free(ev);
	return r;
}
