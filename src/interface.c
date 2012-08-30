#include "game.h"

void iface_init(void) {
	initscr();
	start_color();
	raw(); // Don't translate.
	noecho();
	keypad(stdscr, TRUE);
	timeout(0);
	curs_set(0); // Invisible cursor

	// Create Colours
	if (COLOR_PAIRS >= 8*8) {
		int i; for (i=0; i<8*8; i++) {
			init_pair(i, i/8, i%8);
		}
	}
}
void iface_cleanup(void) {
	endwin();
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
			mvaddch(j, i, (t->symbol == '.' && !BIT_ISSET(t->flags, TILE_VISIBLE)) ? ' ' : t->symbol | COLOR_PAIR(t->color) | (A_BOLD * BIT_ISSET(t->flags, TILE_VISIBLE)));
		}
	}
	const coord_t* pc = &game_d.player.pos;
	mvaddch(pc->y, pc->x, '@' | A_BOLD);
}
