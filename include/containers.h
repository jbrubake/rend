#ifndef _STACK_H_
#define _STACK_H_

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "types.h"

#define containerof(ptr, type, member) ( (type *)( (void *)(ptr) - offsetof(type,member) ) )

typedef void (*free_func)(void*);

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

struct reflist_t;
struct reflist_node_t;

typedef struct reflist_node_t {
	// Order is important
	struct reflist_t* par;
	struct reflist_node_t *n, *p;
	void* data;
} reflist_node_t;

typedef struct reflist_t {
	reflist_node_t *f, *l;
} reflist_t;

#define reflist_init(esize) (reflist_t){0, 0}
reflist_node_t*  reflist_add     (reflist_t* ll,      void* data);
reflist_node_t*  reflist_addafter(reflist_node_t* ln, void* data);
reflist_node_t*  reflist_addprev (reflist_node_t* ln, void* data);
void*            reflist_remove  (reflist_node_t** hln);
void             reflist_clean   (reflist_t* ll, free_func f);

/////////////////////////////////////////////////////////////////////////
/*
	Embedded linked list.

	// Declare an object like this:
	struct linkable_t {
		int field1;
		link_t link;
		int field2;
		link_t link2;
		char field3[256];
	} linkable={.link = {0}}, linkable2={.link = {0}};

	// Then you can do this:
	link_addnext(&linkable->link, &linkable2->link) // linkable2 now follows linkable in a list
	// You will have to use link_data() to get at your data if you do it this way.

	// Or make a list object explicitly:
	link_list_t list = list_init(struct linkable_t, link2);
	link_add(&list, &linkable)
	link_add(&list, &linkable2)

	// Then you can just iterate the list:
	link_iter_t iter = link_iter_first(&list)
	while (iter.el) {
		do_something(iter.el)
		link_next(&iter)
	}

	Advantages:
		- Easier to flatten.
		- Possibility of using static memory (as above... no calls to free() or malloc()!)
		- Easily supports multiple lists with the same objects. Order by alphabetical and chronological order, for example.
 */
/////////////////////////////////////////////////////////////////////////

/*
	This is the main linked list. f is the first element, l is the last, and offset is bookkeeping to find the parent object.
 */

typedef struct link_list_t {
	struct link_t *f, *l;
	size_t offset;
} link_list_t;

typedef struct link_t {
	struct link_t * n;
	struct link_t * p;
} link_t;

typedef struct link_iter_t {
	struct link_t * node;
	void* el;
	size_t offset;
} link_iter_t;

void link_addnext(link_t* root, link_t* node);
void link_addprev(link_t* root, link_t* node);
link_t* link_remove(link_t* el);
#define link_data(ptype, elname, el) containerof(el, ptype, elname)

#define link_init(ptype, elname) (link_list_t){0, 0, offsetof(ptype, elname)}
void link_add(link_list_t* h, void* el);
void link_erase(link_list_t* h, void* el);
void link_clean(link_list_t* h);

void link_next(link_iter_t* i);
void link_prev(link_iter_t* i);
#define link_iter_first(h) (link_iter_t){(h)->f, (h)->f?((void*)((h)->f) - (h)->offset):0, (h)->offset}
#define link_iter_last(h) (link_iter_t){(h)->l, (h)->l?((void*)((h)->l) - (h)->offset):0, (h)->offset}
