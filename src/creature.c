#include "creature.h"

enum {
	// Body sections
	PART_HEAD,
	PART_NECK,
	PART_CHEST,
	PART_ABDOMEN,
	PART_PELVIS,
	PART_LEFTLEG,
	PART_RIGHTLEG,
	PART_LEFTARM,
	PART_RIGHTARM,
	PART_LEFTHAND,
	PART_RIGHTHAND,

	// Bones
	PART_SKULL,
	PART_UPPERSPINE,
	PART_LOWERSPINE,
	PART_RIBS,
	PART_PELVICBONE,
	PART_LEFTFEMUR,
	PART_RIGHTFEMUR,

	// Organs
	PART_BRAIN,
	PART_HEART,
	PART_LEFTLUNG,
	PART_RIGHTLUNG,
	PART_SPLEEN,
	PART_GUT,

    PART_NUM
};

void creature_getwounds(entity_id e, entity_l * li) {
    if (!e) {return;}
    part_t * const p = component_get(e, CPT_PART); assert(p);

    if (p->vitality.f[0] != p->vitality.f[1]) {entity_add(li, e);}
    entity_l it;
    for (it = p->organs; it; it = it->n) {creature_getwounds(it->el, li);}
    for (it = p->children; it; it = it->n) {creature_getwounds(it->el, li);}
}

static inline void print_fraction(fraction_t frac) {
	printf("%d/%d\n", frac.f[0], frac.f[1]);
}

static void print_part(entity_id part) {
    name_t* const n = component_get(part, CPT_NAME); assert(n);
    part_t* const p = component_get(part, CPT_PART); assert(p);
	printf("%s: (%d/%d) [", n->str, p->vitality.f[0], p->vitality.f[1]);
	int k=0;
	if (p->necessary)   {printf(k?" %s":"%s",               "necessary"); k++;}
	if (!p->importance) {printf(k?" %s":"%s",               "important"); k++;}
	if (p->pain)        {printf(k?" pain=%d":"pain=%d",     p->pain);     k++;}
	if (p->bleeds)      {printf(k?" bleeds=%d":"bleeds=%d", p->bleeds);   k++;}
	printf("]\n");
}

static void print_parttree(entity_id part, int depth) {
    name_t * const nm = component_get(part, CPT_NAME); assert(nm);
    part_t * const p  = component_get(part, CPT_PART); assert(p);
	int i; for (i=0; i<depth; i++) {printf("\t");}
	printf(depth?"->%s\n":"%s\n", nm->str);
	entity_l n;
	n = p->organs;
	while (n) {print_parttree(n->el, depth+1); n = n->n;}
	n = p->children;
	while (n) {print_parttree(n->el, depth+1); n = n->n;}
}

static void print_body(entity_id ent) {
    body_t* const b = component_get(ent, CPT_BODY);
    assert(b && "No body component in entity passed to print_body()");
	print_parttree(b->rootpart, 0);
}

// A helper method that clones whole part trees.
static entity_id part_deep_copy(entity_id p) {
    entity_id part = entity_create();
    { // Copy the name
        name_t * const oldname = component_get(p, CPT_NAME); assert(oldname);
        name_t * const newname = component_attach(part, CPT_NAME); assert(newname);
        strcpy(newname->str, oldname->str);
    }

    // Copy the part information
    part_t * const oldpart = component_get(p, CPT_PART); assert(oldpart);
    part_t * const newpart = component_attach(part, CPT_PART);
    component_t base = newpart->base;
    *newpart = *oldpart;
    newpart->children = 0;
    newpart->organs = 0;
    newpart->base = base;

    // Copy the list of child parts recursively.
    entity_l it;
    for (it = oldpart->children; it; it = it->n) {
        entity_add( &newpart->children, part_deep_copy(it->el) );
    }
    for (it = oldpart->organs; it; it = it->n) {
        entity_add( &newpart->organs, part_deep_copy(it->el) );
    }
	return part;
}

// There's only one template, but this function constructs real bodies from the monster library.
void body_from_template(body_t * b, entity_id t) {
    body_t * template = component_get(t, CPT_BODY); assert(template);
	assert(template->rootpart && "No body in template"); // What kind of body template doesn't have at least the one part?
    {
        component_t base = b->base;
        *b = *template;
        b->base = base;
    }
	b->rootpart = part_deep_copy(template->rootpart);
}

static void part_clean(component_t* part) {
    part_t * const p = (part_t *)part;
    while (p->children) {
        entity_destroy(p->children->el);
        entity_del(&p->children);
    }
    while (p->organs) {
        entity_destroy(p->organs->el);
        entity_del(&p->organs);
    }
}

static void body_clean(component_t *x) {
    body_t * const b = (body_t*)x;
	entity_destroy(b->rootpart);
}

// FIXME: Remove this when we refactor eventmanager.h
// Cos it's like... literally a straight copy, and that's unacceptable.
enum {
	GOBBO_REST,
	PLAYER_REST,
};

//////////////////////////////////////////////////////////////////////
// These functions are temporary
//////////////////////////////////////////////////////////////////////

static void part_create(entity_id ent, const char *nm, fraction_t vitality, int necessary, int importance, int pain, int bleeds, int size) {
    name_t * const name = component_attach(ent, CPT_NAME);
    part_t * const part = component_attach(ent, CPT_PART);

    strncpy(name->str, nm, NAME_SIZE);
    *part = (part_t){
        .base       = part->base,
        .vitality   = vitality,
        .necessary  = necessary,
        .importance = importance,
        .pain       = pain,
        .bleeds     = bleeds,
        .size       = size
    };
}

static void part_relate(int rtype, entity_id parent, entity_id child) {
    part_t * const p = component_get(parent, CPT_PART); assert(p);

    entity_add(rtype ? &p->organs : &p->children, child);
}

void creature_test_init() {
    entity_id parts[PART_NUM];
    int i;
    for (i=0; i<PART_NUM; i++) {parts[i] = entity_create();}

#define PART(part, name, health, ...) part_create(parts[part], name, FRACT(health, health), __VA_ARGS__)
#define CONNECTED(parent, child) part_relate(0, parts[parent], parts[child])
#define CONTAINS(parent, child) part_relate(1, parts[parent], parts[child])

	// PART(Name, health, necessary, importance level, pain, bleeding)
	PART(PART_HEAD,       "Head",         16, 0, 0,  8, 4,   8);
	PART(PART_NECK,       "Neck",         12, 0, 0,  8, 16,  8);
	PART(PART_CHEST,      "Chest",        64, 0, 0,  8, 4,  64);
	PART(PART_ABDOMEN,    "Abdomen",      64, 0, 0,  8, 4,  64);
	PART(PART_PELVIS,     "Pelvis",       48, 0, 0,  8, 4,  16);
	PART(PART_LEFTLEG,    "Left Leg",     32, 0, 0,  8, 6,  16);
	PART(PART_RIGHTLEG,   "Right Leg",    32, 0, 0,  8, 6,  16);
	PART(PART_LEFTARM,    "Left Arm",     24, 0, 0,  8, 6,  12);
	PART(PART_RIGHTARM,   "Right Arm",    24, 0, 0,  8, 6,  12);
	PART(PART_LEFTHAND,   "Left Hand",     8, 0, 0,  8, 2,   6);
//	PART(PART_RIGHTHAND,  "Right Hand",    8, 0, 0,  8, 2,   6);
    part_create(parts[PART_RIGHTHAND], "Right Hand", FRACT(-1, 8), 0, 0,  8, 2,   6);

	PART(PART_SKULL,      "Skull",        12, 0, 1, 16,  0,  4);
	PART(PART_UPPERSPINE, "Upper Spine",   8, 0, 1, 12,  0,  4);
	PART(PART_LOWERSPINE, "Lower Spine",   8, 0, 1, 12,  0,  6);
	PART(PART_RIBS,       "Ribs",         12, 0, 1, 16,  0, 32);
	PART(PART_PELVICBONE, "Pelvic Bone",  20, 0, 1, 12,  0, 12);
	PART(PART_LEFTFEMUR,  "Left Femur",   20, 0, 1, 16, 16, 12);
	PART(PART_RIGHTFEMUR, "Right Femur",  20, 0, 1, 16, 16, 12);

	PART(PART_BRAIN,      "Brain",         8, 1, 1,  0,  4,  4);
	PART(PART_HEART,      "Heart",        12, 1, 1,  8, 32,  4);
	PART(PART_LEFTLUNG,   "Left Lung",    12, 1, 1,  8,  4,  4);
	PART(PART_RIGHTLUNG,  "Right Lung",   12, 1, 1,  8,  4,  4);
	PART(PART_SPLEEN,     "Spleen",       12, 0, 1,  8, 24,  4);
	PART(PART_GUT,        "Gut",          24, 0, 1, 12, 16,  8);

    // Main Body
    CONNECTED(PART_HEAD, PART_NECK);
    CONNECTED(PART_NECK, PART_CHEST);
    CONNECTED(PART_CHEST, PART_ABDOMEN);
    CONNECTED(PART_ABDOMEN, PART_PELVIS);
    CONNECTED(PART_PELVIS, PART_LEFTLEG);
	CONNECTED(PART_PELVIS, PART_RIGHTLEG);

    // Right arm
	CONNECTED(PART_CHEST, PART_RIGHTARM);
	CONNECTED(PART_RIGHTARM, PART_RIGHTHAND);

	// Left arm
	CONNECTED(PART_CHEST, PART_LEFTARM);
	CONNECTED(PART_LEFTARM, PART_LEFTHAND);

	// Organs
	CONTAINS(PART_SKULL, PART_BRAIN);
	CONTAINS(PART_CHEST, PART_HEART);
	CONTAINS(PART_CHEST, PART_LEFTLUNG);
	CONTAINS(PART_CHEST, PART_RIGHTLUNG);
	CONTAINS(PART_ABDOMEN, PART_SPLEEN);
	CONTAINS(PART_ABDOMEN, PART_GUT);

    // Print results
    for (i=0; i<sizeof(parts)/sizeof(*parts); i++) {print_part(parts[i]);}
	game_d.transient.humanoid = entity_create();
    body_t * const body = component_attach(game_d.transient.humanoid, CPT_BODY);
	body->rootpart = parts[PART_HEAD];
    body->strength = body->endurance = body->agility = 65;
}

void creature_test_cleanup() {entity_destroy(game_d.transient.humanoid);}

creature_t *humanoid_generator(const char * name, coord_t p, int priority) {
    // Create the humanoid.
    entity_id humanoid = entity_create();

    // Give the humanoid a name.
    {
        name_t * const nm = component_attach(humanoid, CPT_NAME);
        strncpy(nm->str, name, NAME_SIZE);
    }

    // Give the humanoid a body.
    body_t * const body = component_attach(humanoid, CPT_BODY);
	body_from_template(body, game_d.transient.humanoid);
	print_body(humanoid);

    // Make it into a creature.
	creature_t * const g = component_attach(humanoid, CPT_CREATURE);
	g->concentration = FRACT(10, 10);
	g->stamina = FRACT(10, 10);
	g->guard = FRACT(10, 10);
//	printf("Guard: "); print_fraction(g->guard);
//	printf("Concentration: "); print_fraction(g->concentration);
//	printf("Stamina: "); print_fraction(g->stamina);

    // Give the humanoid a position.
    {
        pos_t * const pos = component_attach(humanoid, CPT_POS);
        pos->pos = p;
    }

    {
        symbol_t * const sym = component_attach(humanoid, CPT_SYMBOL);
        sym->symbol = 'g';
        sym->color = iface_color(COLOR_WHITE, COLOR_BLACK);
    }

    // Put in a rest event.
    // FIXME: Should use a controller component
	event_rest_t* ev = ref_alloc(sizeof(*ev));
	ev->event.type = GOBBO_REST;
	ev->event.priority = priority; // Establish the time stamp of the creature's next action (could be now).
	ev->creature = humanoid;
	entity_add(&game_d.goblins, humanoid);
	heap_push(&game_d.pqueue, ev);
	return g;
}

// List of component managers related to creatures.
manager_t cpt_part = {.type = CPT_PART, .size = sizeof(part_t), .cpt_destroy = part_clean};
manager_t cpt_soul = {.type = CPT_SOUL, .size = sizeof(soul_t)};
manager_t cpt_body = {.type = CPT_BODY, .size = sizeof(body_t), .cpt_destroy = body_clean};
manager_t cpt_creature = {.type = CPT_CREATURE, .size = sizeof(creature_t)};
