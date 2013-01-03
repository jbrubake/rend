#include <curses.h>
#include "game.h"

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#define iface_color(fg, bg) ((fg<<3) + bg)

#define WINDOW_SIZEX 80
#define WINDOW_SIZEY 25
#define MAP_SIZEX 60
#define MAP_SIZEY 20

typedef struct log_line_t {
	link_t log_order;
	char msg[0];
} log_line_t;

typedef struct iface_t {
	link_list_t displaylist; // A list of those windows currently displayed.
	WINDOW* win_main;
	WINDOW* win_trace;
	WINDOW* win_creature;
	WINDOW* win_health;
	link_list_t log;
	uint logN;
} iface_t;

void iface_init      (void);
void iface_cleanup   (void);

void iface_setup     (void);

void iface_hbar      (WINDOW * const win, int y, int x, fraction_t f, int col);
void iface_printline (const char * str);

void iface_map_pane  (map_t* m);
void iface_info_pane (void);
void iface_trace_pane(void);

void iface_swap      (void);

int iface_next_key   (void);

#endif
