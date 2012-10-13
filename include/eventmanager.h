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

static int gobbo_move(void* event) {
	event_rest_t * const ev = event;
	actor_t * const g = ev->creature;
	const coord_t * const w = where + kiss_rand()%8;
	tile_t* t = map_get_tile(game_d.map, g->pos.x + w->x, g->pos.y + w->y);
	if ( t && !BIT_ISSET(t->flags, TILE_OCCUPIED) && BIT_ISSET(t->flags, TILE_WALKABLE) ) {
		g->pos.x += w->x;
		g->pos.y += w->y;
	}

	ref_copy(ev); // Make sure the event is not deleted.
	ev->event.priority = game_d.time + 100; // Renew
	heap_push(&game_d.pqueue, ev);
	char buffer[56];
	snprintf(buffer, sizeof(buffer), "Goblin moved!");
	iface_printline(buffer);
	return 0;
}

static int player_move(void* event) {
	event_t * const ev = event;
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
	char buffer[56];
	snprintf(buffer, sizeof(buffer), "Player moved to: %d %d", game_d.player.pos.x, game_d.player.pos.y);
	iface_printline(buffer);
	return 0;
}

typedef int (*event_func)(void*);
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
