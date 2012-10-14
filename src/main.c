#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "game.h"
#include "creature.h"

// FIXME: Shouldn't be here
int arifunc (const char * x) {
	static int limit = 100;
	if (limit) {limit--; return 0;} // Ignore 100 errors
	return 2;
}

void outfunc(int x) {
	static int o = 10000;
	if (o>0) {printf("%c", x);}
	o--;
}

typedef struct linkable_t {
	int f;
	link_t l;
} linkable_t;

void* int_cpy(void * x) {int* y = malloc(sizeof(*y)); *y = *(int*)x; return y;}
int main() {
	mwInit();
	mwSetAriFunc(arifunc);
	mwSetOutFunc(outfunc);

	reflist_t rl = reflist_init();
	reflist_t rln;
	int n[] = {0,12,5,6,14};
	int i;
	for (i=0; i<sizeof(n)/sizeof(*n); i++) {reflist_add(&rl, n+i);}
	reflist_copy(&rln, &rl, int_cpy);
	reflist_node_t * iter = rln.f;
	while (iter) {
		printf("%d\n", *(int*)iter->data);
		iter = iter->n;
	}
	reflist_clean(&rl, 0);
	reflist_clean(&rln, free);

/*
	creature_test_init();
	creature_t *gobby = humanoid_generator((coord_t){0, 0});
	creature_destroyer(gobby);
	creature_test_cleanup();
*/

/*
	game_init();
	iface_setup();
	game_loop();
	game_clean();
*/
	mwTerm();
	return 0;
}
