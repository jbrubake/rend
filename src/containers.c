#include "containers.h"
#include "memwatch.h"

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

dllist_t* dllist_add(dllist_t** hdl, void* data) {
	dllist_t *dl = *hdl;
	dllist_t *t = malloc(sizeof(*t));
	*t = (dllist_t){data, 0, 0};
	if (!dl) {*hdl = t; return t;}
	if (dl->n) {dl->n->p = t;}
	t->n = dl->n;
	dl->n = t;
	t->p = dl;
	return t;
}

dllist_t* dllist_addprev(dllist_t** hdl, void* data) {
	dllist_t *dl = *hdl;
	dllist_t *t = malloc(sizeof(*t));
	*t = (dllist_t){data, 0, 0};
	if (!dl) {*hdl = t; return t;}
	if (dl->p) {dl->p->n = t;}
	t->p = dl->p;
	dl->p = t;
	t->n = dl;
	return t;
}

dllist_t* dllist_remove (dllist_t** hdl) {
	dllist_t *t = *hdl;
	if (!t) {return 0;}
	dllist_t* const n = t->n;
	dllist_t* const p = t->p;
	if (p) {p->n = n;}
	if (n) {n->p = p;}
	free(t);
	if (p) {return *hdl = p;}
	return *hdl = n;
}

void dllist_clean  (dllist_t* dl) {while (dl) {                dllist_remove(&dl);}}
void dllist_cleanf (dllist_t* dl) {while (dl) {free(dl->data); dllist_remove(&dl);}}

#ifdef UNITDLLIST

#include "debug.h"

int main() {
	int x[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	dllist_t *l, *i, *j;

	// Test 1: Empty list.
	l = dllist_init();
	dllist_clean(l);
	DEBUG_INFO("\n\tTest 1 Complete\n");

	// Test 2: Make sure elements get added the expected way
	l = dllist_init();
	dllist_add(&l, x+0);
	dllist_add(&l, x+1);
	dllist_add(&l, x+2);
	dllist_add(&l, x+3);
	dllist_add(&l, x+4);
	i = l;
	printf("[ ");
	while (i) {
		printf("%d ", *(int*)i->data);
		i = i->n;
	}
	printf("]\n");
	printf("\n\tTest 2 Expect: [ 0 4 3 2 1 ]\n");
	dllist_clean(l);

	// Test 3: Make sure elements get added the expected way
	l = dllist_init();
	i = dllist_add(&l, x+0);
	i = dllist_add(&i, x+1);
	i = dllist_add(&i, x+2);
	i = dllist_add(&i, x+3);
	i = dllist_add(&i, x+4);
	i = l;
	printf("[ ");
	while (i) {
		printf("%d ", *(int*)i->data);
		i = i->n;
	}
	printf("]\n");
	printf("\n\tTest 3 Expect: [ 0 1 2 3 4 ]\n");
	dllist_clean(l);

	// Test 4: Make sure elements get added the expected way
	l = dllist_init();
	i = dllist_add(&l, x+0);
	i = dllist_add(&i, x+1);
	i = dllist_add(&i, x+2);
	i = dllist_add(&i, x+3);
	i = dllist_add(&i, x+4);
	i = l->n->n;
	dllist_add(&i, x+5);
	i = l;
	printf("[ ");
	while (i) {
		printf("%d ", *(int*)i->data);
		i = i->n;
	}
	printf("]\n");
	printf("\n\tTest 4 Expect: [ 0 1 2 5 3 4 ]\n");
	dllist_clean(l);

	// Test 5: Make sure elements get added the expected way
	l = dllist_init();
	i = dllist_add(&l, x+0);
	i = dllist_add(&i, x+1);
	i = dllist_add(&i, x+2);
	i = dllist_add(&i, x+3);
	i = dllist_add(&i, x+4);
	i = l->n->n;
	dllist_addprev(&i, x+5);
	i = l;
	printf("[ ");
	while (i) {
		printf("%d ", *(int*)i->data);
		j = i;
		i = i->n;
	}
	printf("]\n");
	printf("[ ");
	while (j) {
		printf("%d ", *(int*)j->data);
		j = j->p;
	}
	printf("]\n");
	printf("\n\tTest 5 Expect: [ 0 1 5 2 3 4 ] and reverse\n");
	dllist_clean(l);

	return 0;
}

#endif
