#include "game.h"

#ifndef _CREATURE_H_
#define _CREATURE_H_

typedef union fraction_t {
    struct {
        int v;
        int max;
    };
    int f[2];
} fraction_t;
#define FRACT(x, y) (fraction_t){.f = {x, y}}

typedef struct part_t {
	component_t base;     // The base component.

	fraction_t vitality;  // The current health of the part.

	// Could be bitfields.
	u8 necessary;         // There are currently no health dependencies. Instead, a body dies when a necessary part is destroyed.
	u8 importance;        // Display this part in body part lists, even if not wounded (0 = yes, 1 = only when wounded, >=2 only in advanced mode)
	u8 pain;
	u8 size;              // A weigth for the probability that a successful attack strikes this target.
	u16 bleeds;           // How fast does this part bleed?

	entity_l children;    // Any associated children in the skeleton.
	entity_l organs;      // Any organs found internal to the part.
} part_t;

typedef struct soul_t {
    component_t base;

	u8 intelligence;
	u8 will;
	u8 charisma;
} soul_t;

typedef struct body_t {
    component_t base;

    entity_id rootpart;
	u8 strength;
	u8 endurance;
	u8 agility;
} body_t;
typedef body_t template_t;

typedef struct creature_t
{
    component_t base;

	// Dynamic attributes
	fraction_t guard;
	fraction_t concentration;
	fraction_t stamina;
} creature_t;

extern manager_t cpt_part;
extern manager_t cpt_soul;
extern manager_t cpt_body;
extern manager_t cpt_creature;

enum {
	CREATURE_HUMAN,
	CREATIRE_GOBLIN,
};

// Create a ref_t counted body from a template.
void body_from_template(body_t * b, entity_id t);

// Some very temporary functions.
void creature_test_init();
void creature_test_cleanup();
// FIXME: Should this be static?
creature_t *humanoid_generator(const char * name, coord_t p, int priority);

#endif
