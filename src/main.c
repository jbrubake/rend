#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "game.h"

// FIXME: Shouldn't be here
int arifunc (const char * x) {
	static int limit = 100;
	if (limit) {limit--; return 0;} // Ignore 100 errors
	return 2;
}

void outfunc(int x) {
	static int o = 4096;
	if (o>0) {printf("%c", x);}
	o--;
}

void* int_cpy(void * x) {int* y = malloc(sizeof(*y)); *y = *(int*)x; return y;}
int main() {
	mwInit();
	mwSetAriFunc(arifunc);
	mwSetOutFunc(outfunc);

	game_init();
	iface_setup();
	game_loop();
	game_clean();
	creature_test_cleanup();

	mwTerm();
	return 0;
}
