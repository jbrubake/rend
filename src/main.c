#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "game.h"

int main() {
	game_init();
	game_loop();
	game_clean();
	return 0;
}
