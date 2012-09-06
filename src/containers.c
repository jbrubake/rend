#include "containers.h"
#include "memwatch.h"
#include <string.h>

// stk_t

stk_t* stk_init(uint esize, uint isize) {
	stk_t* x = malloc(sizeof(stk_t) + esize * isize);
	x->maxsize = isize;
	x->esize = esize;
	x->used = 0;
	return x;
}

void* stk_index (stk_t* stk, uint i) {return stk->memory + stk->esize * i;}

uint stk_addmany (stk_t** hstk, uint many) {
	const uint r = (*hstk)->used;
	(*hstk)->used += many;
	uint resize = 0;
	while ((*hstk)->used >= (*hstk)->maxsize) {
		// Double stk size
		(*hstk)->maxsize *= 2;
		resize=1;
	}
	if (resize) {*hstk = realloc(*hstk, sizeof(stk_t) + (*hstk)->esize * (*hstk)->maxsize);}
	return r;
}

// varr_t

varr_t* varr_init  (uint esize, uint isize) {
	varr_t* r = malloc(sizeof(varr_t) + esize * isize);
	r->esize = esize;
	r->maxsize = isize;
	r->used = 0;
	return r;
}

void*   varr_index (varr_t*  varr,    uint i) {return varr->memory + i * varr->esize;}

uint    varr_add   (varr_t** hvarr) {
	(*hvarr)->used++;
	int resize = 0;
	while ((*hvarr)->used >= (*hvarr)->maxsize) {(*hvarr)->maxsize *= 2; resize = 1;}
	if (resize) {*hvarr = realloc(*hvarr, sizeof(varr_t) + (*hvarr)->maxsize * (*hvarr)->esize);}
	return (*hvarr)->used-1;
}

void    varr_remove(varr_t** hvarr,     uint i) {
	varr_t* const v = *hvarr;
	memcpy(v->memory + i*v->esize, v->memory + (v->used-1)*v->esize, v->esize);
	v->used--;
	if (v->used*4 < v->maxsize) {v->maxsize = v->used*2; *hvarr = realloc(*hvarr, sizeof(varr_t) + (*hvarr)->maxsize * (*hvarr)->esize);}
}

#ifdef UNITVARR

#include <stdio.h>

int main() {
	int* x;
	varr_t* v = varr_init(sizeof(int), 60);
	varr_add(&v);
	varr_add(&v);
	varr_add(&v);
	varr_add(&v);
	x = varr(v);
	x[0] = 0;
	x[1] = 1;
	x[2] = 2;
	x[3] = 3;
	varr_add(&v);
	varr_add(&v);
	varr_add(&v);
	varr_add(&v);
	x = varr(v);
	x[4] = 4;
	x[5] = 5;
	x[6] = 6;
	x[7] = 7;
	varr_remove(&v, 4);
	x = varr(v);
	int i; for (i=0; i<7; i++) {printf("%d\n", x[i]);}
	printf("Used: %d\n", v->used);
	printf("Maxsize: %d\n", v->maxsize);

	return 0;
}

#endif
void llist_add(llist_t* ll, const void* data) {
	llist_node_t * const x = malloc(sizeof(llist_node_t) + ll->esize);
	*x = (llist_node_t){ll};
	memcpy(x->data, data, ll->esize);
	if (!ll->f) {ll->f = ll->l = x; return;}
	x->p = ll->l;
	x->p->n = x;
	ll->l = x;
	return;
}
llist_node_t* llist_addafter(llist_node_t* ln, const void* data) {
	llist_t * const ll = ln->par;
	llist_node_t * const x = malloc(sizeof(llist_node_t) + ll->esize);
	*x = (llist_node_t){ll};
	memcpy(x->data, data, ll->esize);
	x->p = ln;
	x->n = ln->n;
	if (x->n) {x->n->p = x;}
	if (x->p) {x->p->n = x;}
	if (ll->l == x->p) {ll->l = x;}
	return x;
}

llist_node_t* llist_addprev (llist_node_t* ln, const void* data) {
	llist_t * const ll = ln->par;
	llist_node_t * const x = malloc(sizeof(llist_node_t) + ll->esize);
	*x = (llist_node_t){ll};
	memcpy(x->data, data, ll->esize);
	x->n = ln;
	x->p = ln->p;
	if (x->n) {x->n->p = x;}
	if (x->p) {x->p->n = x;}
	if (ll->f == x->n) {ll->f = x;}
	return x;
}

void  llist_remove  (llist_node_t** hln) {
	llist_node_t * const ln = *hln;
	llist_t * const ll = ln->par;
	llist_node_t * const p = ln->p;
	llist_node_t * const n = ln->n;
	if (p) {p->n = n;}
	if (n) {n->p = p;}

	if (ll->f == ln) {ll->f = ln->n;}
	if (ll->l == ln) {ll->l = ln->p;}

	free(ln);
	*hln = p ? p : n;
}

void llist_clean   (llist_t* ll) {
	llist_node_t * ln = ll->f;
	while (ln) {llist_remove(&ln);}
}

#ifdef UNITLLIST
#include <stdio.h>
#include <string.h>
int main() {
	llist_t ll; llist_node_t *iter, *iter2; int e;

	// Test 1: Empty List
	ll = llist_init(sizeof(int));
	llist_clean(&ll);
	printf("Test 1 complete\n");

	// Test 2: Adding elements.
	ll = llist_init(sizeof(int));
	e = 0; llist_add(&ll, &e);
	e = 1; llist_add(&ll, &e);
	e = 2; llist_add(&ll, &e);
	e = 3; llist_add(&ll, &e);
	e = 4; llist_add(&ll, &e);
	iter = ll.f;
	printf("[ ");
	while (iter) {printf("%d ", *(int*)iter->data); iter = iter->n;}
	printf("]\n");
	llist_clean(&ll);
	printf("Test 2 complete\n");

	// Test 3: Removing elements.
	ll = llist_init(sizeof(int));
	e = 0; llist_add(&ll, &e);
	e = 1; llist_add(&ll, &e);
	e = 2; llist_add(&ll, &e);
	iter2 = ll.l;
	e = 3; llist_add(&ll, &e);
	e = 4; llist_add(&ll, &e);
	iter = ll.f;
	printf("Before: [ ");
	while (iter) {printf("%d ", *(int*)iter->data); iter = iter->n;}
	printf("]\n");
	llist_remove(&iter2);
	iter = ll.f;
	printf("After: [ ");
	while (iter) {printf("%d ", *(int*)iter->data); iter = iter->n;}
	printf("]\n");
	llist_clean(&ll);
	printf("Test 3 complete\n");

	// Test 4: Adding elements in reverse order
	ll = llist_init(sizeof(int));
	e = 0; llist_add(&ll, &e);
	iter = ll.f;
	e = 1; iter = llist_addprev(iter, &e);
	e = 2; iter = llist_addprev(iter, &e);
	e = 3; iter = llist_addprev(iter, &e);
	e = 4; iter = llist_addprev(iter, &e);
	iter = ll.f;
	printf("[ ");
	while (iter) {printf("%d ", *(int*)iter->data); iter = iter->n;}
	printf("]\n");
	llist_clean(&ll);
	printf("Test 4 complete\n");

	return 0;
}

#endif
