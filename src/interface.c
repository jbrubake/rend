#include "game.h"

static WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
//	box(local_win, 0, 0);
//	wborder(local_win,  ls,  rs,  ts,  bs,  tl,  tr,  bl,  br);
//	wborder(local_win, '|', '|', '-', '-', '+', '+', '+', '+');
//	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

static void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}

void iface_printline(const char * str) {
	size_t bsize = strlen(str);
	log_line_t *line = malloc(sizeof(*line) + bsize + 1);
	line->log_order = (link_t){0};

	strcpy(line->msg, str);

	link_add(&game_d.iface.log, line);
}

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

	iface_t * const iface = &game_d.iface;
	iface->log = link_init(log_line_t, log_order);
	iface_printline("Hello from the event log!");
}

#define COORD2INDEX(x, y, xmax) ((x) + (xmax) * (y))
void iface_map_pane(map_t* m) {
	WINDOW * const win = game_d.iface.main;

	int i, j; tile_t* t;
	for (i=0; i<m->size[0]; i++) {
		for (j=0; j<m->size[1]; j++) {
			t = map_get_tile(m, i, j);
			mvwaddch(win, j, i, t->symbol == '.' && (!BIT_ISSET(t->flags, TILE_VISIBLE)) ? ' ' : t->symbol | COLOR_PAIR(t->color) | (A_BOLD * BIT_ISSET(t->flags, TILE_VISIBLE)));
		}
	}
	const coord_t* pc = &game_d.player.pos;
	mvwaddch(win, pc->y, pc->x, '@' | A_BOLD);
	if (game_d.fov.mode) {
		t = map_get_tile(game_d.map, game_d.fov.k.x, game_d.fov.k.y);
		if (!t) {return;}
		mvwaddch(win, game_d.fov.k.y, game_d.fov.k.x, '%' | COLOR_PAIR(t->color) | A_BOLD);
	}
	reflist_node_t *g = game_d.goblins.f;
	while (g) {
		actor_t* const actor = g->data;
		t = map_get_tile(game_d.map, actor->pos.x, actor->pos.y);
		if (!t) {g = g->n; continue;}
		if (BIT_ISSET(t->flags, TILE_VISIBLE)) {mvwaddch(win, actor->pos.y, actor->pos.x, actor->symbol | COLOR_PAIR(actor->color));}
		g = g->n;
	}
}


void iface_info_pane(void) {
	WINDOW * const win = game_d.iface.right;

	mvwprintw(win, 1, 1, "Character");
	mvwprintw(win, 2, 1, "information");
}

void iface_trace_pane(void) {
	WINDOW * const win = game_d.iface.bottom;
	int Xs, Ys;
	getmaxyx(win, Ys, Xs);
	werase(win);
	
	link_iter_t iter = link_iter_last(&game_d.iface.log);
	int i; for (i=0; i<Ys && iter.el; i++, link_prev(&iter))
	{
		log_line_t* const line = iter.el;
		mvwaddstr(win, Ys-i-1, 1, line->msg);
	}
}
#include <stdio.h>
void iface_cleanup(void) {
	/*link_iter_t * iter = link_iter_first()
	link_t *iter = 0, *last = game_d.iface.log.f;
	while (last) {
		iter = last->n;
		free(link_data(log_line_t, log_order, last));
		last = iter;
	}*/

	destroy_win(game_d.iface.bottom);
	destroy_win(game_d.iface.right);
	destroy_win(game_d.iface.main);

	endwin();
	// FIXME: This is a really stupid way to take care of this bug. Like... really really just extremely bad.
	void dummy_free(void* x) {free(x);} // We need this because of memwatch.
	link_clean(&game_d.iface.log, dummy_free);
}

void iface_swap() {
	wrefresh(game_d.iface.bottom);
	wrefresh(game_d.iface.right);
	wrefresh(game_d.iface.main);
//	refresh();
}

// FIXME: This comment line is longer than 80 characters. You should probably delete this line. It's seriously long, and serves no purpose at all.
int iface_next_key (void) {
	int c = getch();
	return c != ERR ? c : -1;
}

void iface_setup(void) {
	resize_term(25, 80);
	game_d.iface.bottom = create_newwin( 5, 80, 20,  0);
	game_d.iface.right  = create_newwin(20, 15,  0, 65);
	game_d.iface.main   = create_newwin(20, 65,  0,  0);
}
