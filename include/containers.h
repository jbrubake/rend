#ifndef _STACK_H_
#define _STACK_H_

#include <stdlib.h>
#include <string.h>

#include "types.h"

typedef struct stk_t {
	ushort esize;
	ushort maxsize;
	uint used;
	char memory[0];
} stk_t;

stk_t*  stk_init    (uint    esize,   uint isize);
void*   stk_index   (stk_t*  stk,     uint i);
uint    stk_addmany (stk_t** hstk,    uint many);
#define stk_add(hstk) stk_addmany(hstk, 1)
#define stk_clean(stk) free(stk)
#define stk(s)      (void*)(v->memory)

typedef struct varr_t {
	ushort esize;
	ushort maxsize;
	uint used;
	char memory[0];
} varr_t;

varr_t* varr_init  (uint esize, uint isize);
void*   varr_index (varr_t*  varr,    uint i);
uint    varr_add   (varr_t** hvarr);
void    varr_remove(varr_t** hvarr,     uint i);
#define varr_clean(varr) free(varr)
#define varr(v) (void*)(v->memory)

struct llist_node_t;
struct llist_t;

typedef struct llist_node_t {
	// Order is important
	struct llist_t* par; // Parent list.
	struct llist_node_t *n, *p;
	char data[0];
} llist_node_t;

typedef struct llist_t {
	// Order is important
	int esize;
	llist_node_t *f, *l;
} llist_t;

#define llist_init(esize) (llist_t){esize, 0, 0}
void          llist_add     (llist_t* ll,      const void* data);
llist_node_t* llist_addafter(llist_node_t* ln, const void* data);
llist_node_t* llist_addprev (llist_node_t* ln, const void* data);
void          llist_remove  (llist_node_t** hln);
void          llist_clean   (llist_t* ll);

#endif
