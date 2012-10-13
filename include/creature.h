#ifndef _CREATURE_H_
#define _CREATURE_H_

#include "game.h"

typedef struct fraction_t {
	int v;
	int max;
} fraction_t;
#define FRACT(x, y) (struct fraction_t){x, y}

typedef struct part_t {
	ref_t ref;            // For ref-counting
	const char* name;     // The name of the part. "Heart" for example.
	fraction_t vitality;  // The current health of the part.

	// Could be bitfields. Currently, there is no point (would take a word anyways).
	u8 necessary;         // There are currently no health dependencies. Instead, a body dies when a necessary part is destroyed.
	u8 importance;        // Display this part in body part lists, even if not wounded (0 = yes, 1 = only when wounded, >=2 only in advanced mode)
	u8 pain;
	u8 size;              // A weigth for the probability that a successful attack strikes this target.
	u16 bleeds;           // How fast does this part bleed?

	reflist_t children;   // Any associated children in the skeleton.
	reflist_t organs;     // Any organs found internal to the part.
} part_t;

typedef struct body_t {
	part_t* rootpart;
	fraction_t guard;
	fraction_t concentration;
	fraction_t stamina;
} body_t;

typedef struct creature_t
{
	uint type;
	body_t *body;
//	soul_t *soul;
} creature_t;

enum {
	CREATURE_HUMAN,
	CREATIRE_GOBLIN,
};

void creature_test();

#endif
