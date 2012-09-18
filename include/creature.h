#ifndef _CREATURE_H_
#define _CREATURE_H_

#include "game.h"

typedef struct fraction_t {
	int v;
	int max;
} fraction_t;

typedef struct part_t {
	const char* name;
	fraction_t vitality;
	unsigned natural:1;
	unsigned necessary:1;
}

typedef struct body_t {
	
}

typedef struct creature_t {
	ref_t ref;

} creature_t;

creature_t* creature_init() {
}

#endif
