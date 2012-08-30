#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <curses.h>
#include "game.h"

#define iface_color(fg, bg) ((fg<<3) + bg)

void iface_init     (void);
void iface_cleanup  (void);

void iface_drawmap  (map_t* m);

void iface_swap     (void);

int iface_next_key  (void);

#endif
