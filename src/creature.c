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
	PART_GUT
};
#define PART(nm, hlth, nec, imp, pn, bld) {.name = nm, .vitality={hlth, hlth}, .necessary=nec, .importance=imp, .pain=pn, .bleeds=bld}
static part_t parts[] = {
	// PART(Name, health, necessary, importance level, pain, bleeding)
	PART("Head",         16, 0, 0,  8, 4),
	PART("Neck",         12, 0, 0,  8, 16),
	PART("Chest",        64, 0, 0,  8, 4),
	PART("Abdomen",      64, 0, 0,  8, 4),
	PART("Pelvis",       48, 0, 0,  8, 4),
	PART("Left Leg",     32, 0, 0,  8, 6),
	PART("Right Leg",    32, 0, 0,  8, 6),
	PART("Left Arm",     24, 0, 0,  8, 6),
	PART("Right Arm",    24, 0, 0,  8, 6),
	PART("Left Hand",     8, 0, 0,  8, 2),
	PART("Right Hand",    8, 0, 0,  8, 2),

	PART("Skull",        12, 0, 1, 16,  0),
	PART("Upper Spine",   8, 0, 1, 12,  0),
	PART("Lower Spine",   8, 0, 1, 12,  0),
	PART("Ribs",         12, 0, 1, 16,  0),
	PART("Pelvic Bone",  20, 0, 1, 12,  0),
	PART("Left Femur",   20, 0, 1, 16, 16),
	PART("Right Femur",  20, 0, 1, 16, 16),

	PART("Brain",         8, 1, 1,  0,  4),
	PART("Heart",        12, 1, 1,  8, 32),
	PART("Left Lung",    12, 1, 1,  8,  4),
	PART("Right Lung",   12, 1, 1,  8,  4),
	PART("Spleen",       12, 0, 1,  8, 24),
	PART("Gut",          24, 0, 1, 12, 16),
};
static struct {int parent; int child;} connections[] = {
	// Main body bits
	{PART_HEAD, PART_NECK},
	{PART_NECK, PART_CHEST},
	{PART_CHEST, PART_ABDOMEN},
	{PART_ABDOMEN, PART_PELVIS},
	{PART_PELVIS, PART_LEFTLEG},
	{PART_PELVIS, PART_RIGHTLEG},

	// Right arm
	{PART_CHEST, PART_RIGHTARM},
	{PART_RIGHTARM, PART_RIGHTHAND},

	// Left arm
	{PART_CHEST, PART_LEFTARM},
	{PART_LEFTARM, PART_LEFTHAND},
};

static struct {int parent; int child;} contains[] = {
	// Organs
	{PART_SKULL, PART_BRAIN},
	{PART_CHEST, PART_HEART},
	{PART_CHEST, PART_LEFTLUNG},
	{PART_CHEST, PART_RIGHTLUNG},
	{PART_ABDOMEN, PART_SPLEEN},
	{PART_ABDOMEN, PART_GUT},
};

static void print_fraction(fraction_t frac) {
	printf("%d/%d\n", frac.v, frac.max);
}

static void print_part(const part_t* p) {
	printf("%s: (%d/%d) [", p->name, p->vitality.v, p->vitality.max);
	int k=0;
	if (p->necessary)   {printf(k?" %s":"%s",               "necessary"); k++;}
	if (!p->importance) {printf(k?" %s":"%s",               "important"); k++;}
	if (p->pain)        {printf(k?" pain=%d":"pain=%d",     p->pain);     k++;}
	if (p->bleeds)      {printf(k?" bleeds=%d":"bleeds=%d", p->bleeds);   k++;}
	printf("]\n");
}

static void print_parttree(const part_t* p, int depth) {
	int i; for (i=0; i<depth; i++) {printf("\t");}
	printf(depth?"->%s\n":"%s\n", p->name);
	reflist_node_t* n;
	n = p->organs.f;
	while (n) {print_parttree(n->data, depth+1); n = n->n;}
	n = p->children.f;
	while (n) {print_parttree(n->data, depth+1); n = n->n;}
}

static void print_body(const body_t* b) {
	print_parttree(b->rootpart, 0);
	printf("Guard: "); print_fraction(b->guard);
	printf("Concentration: "); print_fraction(b->concentration);
	printf("Stamina: "); print_fraction(b->stamina);
}

// A helper method that clones whole part trees.
static void* part_deep_copy(void *x) {
	part_t * const p = x;
	part_t * const pn = ref_alloc(sizeof(*pn));
	*pn = *p; pn->ref.num = 1;
	reflist_copy(&pn->children, &p->children, part_deep_copy);
	reflist_copy(&pn->organs, &p->organs, part_deep_copy);
	return pn;
}

static void part_deep_clean(void *x) {
	part_t * const p = x;
	reflist_clean(&p->children, part_deep_clean);
	reflist_clean(&p->organs, part_deep_clean);
	ref_free(p);
}

// There's only one template, but this function constructs real bodies from the monster library.
body_t * body_from_template(template_t * t) {
	assert(t->rootpart); // What kind of body template doesn't have at least the one part?
	body_t * b = ref_alloc(sizeof(*b));
	*b = *t;
	b->ref.num = 1;
	b->rootpart = part_deep_copy(t->rootpart);

	return b;
}

void body_clean(body_t *b) {
	part_deep_clean(b->rootpart);
	ref_free(b);
}

void creature_test_init() {
	int i;
	for (i=0; i<sizeof(parts)/sizeof(*parts); i++) {
		print_part(parts+i);
	}
	for (i=0; i<sizeof(connections)/sizeof(*connections); i++) {
		reflist_add(&parts[connections[i].parent].children, parts + connections[i].child);
	}
	for (i=0; i<sizeof(contains)/sizeof(*contains); i++) {
		reflist_add(&parts[contains[i].parent].organs, parts + contains[i].child);
	}
	template_t * const body = &game_d.transient.humanoid;
	body->rootpart = parts + PART_HEAD;
	// FIXME: These calculations should really be done more at the creature level, not in the template, but for now it's fine.
	body->concentration = FRACT(10, 10);
	body->stamina = FRACT(10, 10);
	body->guard = FRACT(10, 10);
}

// These functions are basically for debug, but the helpers might see reuse.
creature_t *humanoid_generator(coord_t p) {
	creature_t * goblin = ref_alloc(sizeof(*goblin));
	goblin->body = body_from_template(&game_d.transient.humanoid);
	print_body(goblin->body);
	return goblin;
}
void creature_destroyer(creature_t *c) {
	// hehe
	body_clean(c->body);
	// soul_clean(c->soul);
}

void creature_test_cleanup() {
	int i;
	for (i=0; i<sizeof(parts)/sizeof(*parts); i++) {
		// All statically allocated parts. Just scrub the reflists.
		reflist_clean(&parts[i].children, 0);
		reflist_clean(&parts[i].organs, 0);
	}
}
