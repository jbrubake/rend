#include "game.h"

#include "keymap.h"
#include "eventmanager.h"
#include "control_functions.h"

// HACK: Needed to force kk into being an l-value.
// FIXME: Probably not needed in the long run.
#define KTR(kk) &(struct {int k;}){kk}

static keyhashnode_t keynodes[] = {
	KEY_DEF(KEY_A1      , move_player,      KTR(KEY_A1)),
	KEY_DEF(KEY_A2      , move_player,      KTR(KEY_A2)),
	KEY_DEF(KEY_A3      , move_player,      KTR(KEY_A3)),
	KEY_DEF(KEY_B1      , move_player,      KTR(KEY_B1)),
	KEY_DEF(KEY_B2      , move_player,      KTR(KEY_B2)),
	KEY_DEF(KEY_B3      , move_player,      KTR(KEY_B3)),
	KEY_DEF(KEY_C1      , move_player,      KTR(KEY_C1)),
	KEY_DEF(KEY_C2      , move_player,      KTR(KEY_C2)),
	KEY_DEF(KEY_C3      , move_player,      KTR(KEY_C3)),
	KEY_DEF(KEY_DOWN    , move_player,      KTR(KEY_DOWN)),
	KEY_DEF(KEY_UP      , move_player,      KTR(KEY_UP)),
	KEY_DEF(KEY_LEFT    , move_player,      KTR(KEY_LEFT)),
	KEY_DEF(KEY_RIGHT   , move_player,      KTR(KEY_RIGHT)),
	KEY_DEF('+'         , toggle_view,      KTR('+')),
	KEY_DEF('k'         , toggle_fov_debug, KTR('k')),
	KEY_DEF('a'         , move_fov_debug,   KTR('a')),
	KEY_DEF('d'         , move_fov_debug,   KTR('d')),
	KEY_DEF('w'         , move_fov_debug,   KTR('w')),
	KEY_DEF('s'         , move_fov_debug,   KTR('s')),
	KEY_DEF('z'         , nview_debug,      KTR('z')),
};

//////////////////////////
static void calc_occupancy() {
	map_t* const m = game_d.map;
	int i; for (i=0; i<m->size[0]*m->size[1]; i++) {BIT_UNSET(m->tiles[i].flags, TILE_OCCUPIED);}
	tile_t *t;
	entity_l gn = game_d.goblins;
	while (gn) {
		const pos_t* const gpos = component_get(gn->el, CPT_POS); assert(gpos);
		t = map_get_tile(m, gpos->pos.x, gpos->pos.y);
		if (t) {
			BIT_SET(t->flags, TILE_OCCUPIED);
		}
		gn = gn->n;
	}
    const pos_t * const ppos = component_get(game_d.player, CPT_POS); assert(ppos);
	t = map_get_tile(m, ppos->pos.x, ppos->pos.y);
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

    // Initialize the interface subsystem.
	iface_init();

    // Initialize the entity and component manager.
    entity_init();

	uint i; for (i=0; i<sizeof(keynodes)/sizeof(*keynodes); i++) {
		key_add(keynodes + i);
	}
	game_d.map = map_init(65, 20);
    {
        game_d.player = entity_create();
        pos_t * const p = component_attach(game_d.player, CPT_POS);
        p->pos = (coord_t){1,1};
        name_t * const nm = component_attach(game_d.player, CPT_NAME);
        strncpy(nm->str, "Player", NAME_SIZE);

        // FIXME: Player setup somewhat lacking...
    }

	game_d.pqueue = heap_init(priority_cmp);
	game_d.goblins = 0;

	{
		event_t *ev = ref_alloc(sizeof(*ev));
		ev->type = PLAYER_REST;
		ev->priority = 25;
		heap_push(&game_d.pqueue, ev);
	}

	creature_test_init();

	humanoid_generator("Goblin", (coord_t){ 6,  6}, 50);
	humanoid_generator("Goblin", (coord_t){ 6, 11},100);
	humanoid_generator("Goblin", (coord_t){11,  6},150);
	humanoid_generator("Goblin", (coord_t){11, 11},200);

	return 0;
}

int game_loop() {
	while (1) {
        const pos_t * const p = component_get(game_d.player, CPT_POS); assert(p);
		fov_calc(game_d.map, p->pos, 6);
		calc_occupancy();
		iface_trace_pane();
		iface_info_pane();
		iface_map_pane(game_d.map);
//		iface_health_pane();
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
	entity_l n = game_d.goblins;
	while (n) {
		entity_destroy(n->el);
		entity_del(&n);
	}

    // Delete all entities
    creature_test_cleanup();
    entity_cleanall();
}
