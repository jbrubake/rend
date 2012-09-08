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

//////////////////////////////////////////////////////////////////////////
/*
	Declare a struct like so:
	struct example_t {
		ref_t ref_t_which_you_can_call_whatever_you_want;
		// Normal variables here.
	};

	To allocate, use:
	struct example_t* ex = ref_alloc(sizeof(*ex));

	To free, use:
	ref_free(ex);

	To create a counted reference, use:
	newex = ref_copy(ex);

	newex == ex, but sometimes the return behaviour is useful.

	That's it!
 */
//////////////////////////////////////////////////////////////////////////

typedef struct ref_t {int num;} ref_t;

void* ref_alloc(int size);
void* ref_copy(void* r);
void  ref_free(void* r);

/////////////////////////////////////////////////////////////////////////
/*
	Define a comparator function for x and y:

	int (*heap_cmp)(void* x, void* y)
	Returning -1 for x<y
	Returning +1 for x>y
	Returning  0 for x=y
	The above gives a min-heap.

	Create a heap with:
	heap_t h = heap_init(heap_cmp)

	Then use "heap_push(&h)" to add values and "void* r = heap_pull(&h)" to extract the least element.
	heap_clean() removes values, but will not free their memory on anything like that.
	You can equally just pop all of the values in the heap.
 */
/////////////////////////////////////////////////////////////////////////

typedef int (*heap_cmp)(void* x, void* y);

typedef struct heap_node_t {
	void* data;
	struct heap_node_t *p, *l, *r;
} heap_node_t;

typedef struct heap_t {
	heap_cmp     cmp;
	int N, D;
	heap_node_t *root;
} heap_t;

#define heap_init(cmp) (heap_t){cmp, 0}
void   heap_push    (heap_t* h, void* x);
void*  heap_pop     (heap_t* h);
void   heap_clean   (heap_t* h);

#endif
