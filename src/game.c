#include "game.h"

#include "keymap.h"
#include "eventmanager.h"
#include "control_functions.h"

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

//////////////////////////
static void gobbo_generate(coord_t p, int priority) {
	actor_t* g  = ref_alloc(sizeof(*g));
	event_rest_t* ev = ref_alloc(sizeof(*ev));
	g->symbol = 'g';
	g->color = iface_color(COLOR_WHITE, COLOR_BLACK);
	g->pos = p;
	ev->event.type = GOBBO_REST;
	ev->event.priority = priority;
	ev->creature = g; // FIXME: Could be ref-copied.
	reflist_add(&game_d.goblins, g);
	heap_push(&game_d.pqueue, ev);
}
static void calc_occupancy() {
	map_t* const m = game_d.map;
	int i; for (i=0; i<m->size[0]*m->size[1]; i++) {BIT_UNSET(m->tiles[i].flags, TILE_OCCUPIED);}
	reflist_node_t *gn = game_d.goblins.f;
	tile_t *t;
	while (gn) {
		const actor_t* const g = gn->data;
		t = map_get_tile(m, g->pos.x, g->pos.y);
		if (t) {
			BIT_SET(t->flags, TILE_OCCUPIED);
		}
		gn = gn->n;
	}
	t = map_get_tile(m, game_d.player.pos.x, game_d.player.pos.y);
	if (t) {
		BIT_SET(t->flags, TILE_OCCUPIED);
	}
}
//////////////////////////

static int priority_cmp(void* x, void* y) {
	return ((event_t*)x)->priority - ((event_t*)y)->priority;
}
int game_init() {
	kiss_seed(0);
	iface_init();
	uint i; for (i=0; i<sizeof(keynodes)/sizeof(*keynodes); i++) {
		key_add(keynodes + i);
	}
	game_d.map = map_init(65, 20);
	game_d.player.pos = (coord_t){1,1};

	game_d.pqueue = heap_init(priority_cmp);
	game_d.goblins = reflist_init();

	{
		event_t *ev = ref_alloc(sizeof(*ev));
		ev->type = PLAYER_REST;
		ev->priority = 25;
		heap_push(&game_d.pqueue, ev);
	}

	gobbo_generate((coord_t){ 6,  6}, 50);
	gobbo_generate((coord_t){ 6, 11},100);
	gobbo_generate((coord_t){11,  6},150);
	gobbo_generate((coord_t){11, 11},200);
	return 0;
}

int game_loop() {
	while (1) {
		fov_calc(game_d.map, game_d.player.pos, 6);
		calc_occupancy();
		iface_trace_pane();
		iface_info_pane();
		iface_map_pane(game_d.map);
		iface_swap();

		event_t* ev = heap_pop(&game_d.pqueue);
		game_d.time = ev->priority;
		if (handle_event(ev)) {	return 0; }
	}
	return 0;
}

void game_clean() {
	iface_cleanup();
	map_clean(game_d.map);
	// Clean the event queue
	void* x;
	while (game_d.pqueue.root) {
		x = heap_pop(&game_d.pqueue);
		ref_free(x);
	}
	// Clean the goblin list
	reflist_node_t* n = game_d.goblins.f;
	while (n) {
		ref_free(n->data);
		reflist_remove(&n);
	}
}
