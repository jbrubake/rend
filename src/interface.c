#include "interface.h"

void iface_init(void) {
	initscr();
	start_color();
	raw(); // Don't translate.
	noecho();
	keypad(stdscr, TRUE);
	timeout(0);

	// Create Colours
	if (COLOR_PAIRS >= 8*8) {
		int i; for (i=0; i<8*8; i++) {
			init_pair(i, i/8, i%8);
		}
	}
}
void iface_cleanup(void) {
	endwin();
	printf("Color Pairs: %d\n", COLOR_PAIRS);
}

void iface_swap() {refresh();}
int iface_next_key (void) {
	int c = getch();
	return c != ERR ? c : -1;
}

#define COORD2INDEX(x, y, xmax) ((x) + (xmax) * (y))
void iface_drawmap(map_t* m) {

	int i, j; tile_t* t;
	for (i=0; i<m->size[0]; i++) {
		for (j=0; j<m->size[1]; j++) {
			t = map_get_tile(m, i, j);
			mvaddch(j, i, t->symbol | COLOR_PAIR(t->color));
		}
		init_pair(1, COLOR_RED, COLOR_BLACK);
	}
}
