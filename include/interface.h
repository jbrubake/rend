#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "types.h"
#include "map.h"

enum Key
{
  // Simple keys
  KEY_SIMPLE_BEGIN = 0,

  KEY_UNKNOWN = 0,
  KEY_NUMPAD_0, KEY_NUMPAD_1,
  KEY_NUMPAD_2, KEY_NUMPAD_3,
  KEY_NUMPAD_4, KEY_NUMPAD_5,
  KEY_NUMPAD_6, KEY_NUMPAD_7,
  KEY_NUMPAD_8, KEY_NUMPAD_9,

  KEY_SIMPLE_END,
  KEY_SIMPLE_MASK = 0x0000ffff,
  // Flags
  KEY_SHIFT =	   0x00010000,
  KEY_CONTROL =	 0x00020000,
  KEY_ALT =		 0x00040000,

  KEY_FLAG_MASK =   0xffff0000
};

void iface_init     (void);
void iface_cleanup  (void);

void iface_drawmap  (map_t* m);

void iface_swap     (void);

uint iface_next_key (void);

#endif
