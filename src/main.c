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
	static int o = 1000;
	if (o>0) {printf("%c", x);}
	o--;
}

int main() {
	mwInit();
	mwSetAriFunc(arifunc);
	mwSetOutFunc(outfunc);
	game_init();
	game_loop();
	game_clean();
	mwSetOutFunc(outfunc);
	mwTerm();
	return 0;
}
