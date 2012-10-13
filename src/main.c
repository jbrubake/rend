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
	static int o = 1000;
	if (o>0) {printf("%c", x);}
	o--;
}

typedef struct linkable_t {
	int f;
	link_t l;
} linkable_t;

int main() {

	mwInit();
	mwSetAriFunc(arifunc);
	mwSetOutFunc(outfunc);
	game_init();
	iface_setup();
	game_loop();
	game_clean();
	mwSetOutFunc(outfunc);
	mwTerm();
/* 
	link_list_t list = link_init(linkable_t, l);
	int i; linkable_t *l;
	for (i=0; i<10; i++) {
		l = malloc(sizeof(*l));
		l[i].f = i + 10;
		l[i].l = (link_t){0};
		link_add(&list, l);
	}

	link_iter_t iter = link_iter_first(&list);
	while (iter.el) {
		linkable_t * const el = iter.el;
		printf("%p %p %p %d\n", iter.node, iter.node->p, iter.node->n, el->f);
		link_next(&iter);
	}*/
//	link_clean(&list, free);
//	creature_test();
	return 0;
}
