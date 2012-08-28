#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <windows.h>
#include "interface.h"
#include "map.h"

int main() {
	map_t* themap = map_init(80, 25);

	iface_init();
	iface_drawmap(themap);
	iface_swap();
	while (1) {
		uint i = iface_next_key();
		if (i == KEY_NUMPAD_0) {break;}
	}
	iface_cleanup();

	map_clean(themap);
	return 0;
}
