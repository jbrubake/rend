#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "interface.h"
#include "map.h"

int main() {
	uint maxx, maxy;
	getmaxyx(stdscr, maxy, maxx);
	map_t* themap = map_init(51, 26);

	iface_init();
	iface_drawmap(themap);
	iface_swap();
	while (1) {
		uint c = iface_next_key();
		if (c == 'q') {break;}
	}
	iface_cleanup();

	map_clean(themap);
	return 0;
}
