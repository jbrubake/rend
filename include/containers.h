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

typedef struct dllist_t {
	// Order is important
	void* data;
	struct dllist_t *n, *p;
} dllist_t;

#define dllist_init() (dllist_t*)(0)
dllist_t* dllist_add(dllist_t** hdl, void* data);
dllist_t* dllist_addprev(dllist_t** hdl, void* data);
dllist_t* dllist_remove (dllist_t** hdl);
void      dllist_clean  (dllist_t* dl);
void      dllist_cleanf (dllist_t* dl);

#endif
