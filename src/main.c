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

int main() {
	mwInit();
	mwSetAriFunc(arifunc);
	game_init();
	game_loop();
	game_clean();
	mwTerm();
	return 0;
}
