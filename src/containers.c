#include "containers.h"
#include "memwatch.h"
#include <string.h>

#ifdef DEBUG
#include <assert.h>
#else
#define assert(x)((void) 0)
#endif

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

void* ref_alloc(int size) {
	ref_t* m = malloc(size);
	*m = (ref_t){1};
	return m;
}
void* ref_copy(void* r) {((ref_t*)r)->num++; return r;}
void  ref_free(void* r) {
	((ref_t*)r)->num--;
	if (((ref_t*)r)->num <= 0) {free(r);}
}

#ifdef UNITREF

typedef struct arm_t {
	ref_t ref;
	int i;
	const char* s;
} arm_t;

#include <stdio.h>

int main() {
	arm_t* arm = ref_alloc(sizeof(arm_t));
	arm_t* arms[] = {ref_copy(arm), ref_alloc(sizeof(arm_t))};
	arms[0]->i = 12;
	arms[0]->s = "Left";
	arms[1]->i = 42;
	arms[1]->s = "Right";
	printf("%d, %s\n", arm->i, arm->s);
	printf("%d, %s\n", arms[0]->i, arms[0]->s);
	printf("%d, %s\n", arms[1]->i, arms[1]->s);
	ref_free(arms[0]);
	ref_free(arms[1]);
	ref_free(arm);
	return 0;
}

#endif

////////////////////////////////////////////////////////////////////

static heap_node_t* heap_node_new(heap_node_t* p, void* x) {
	heap_node_t* n = malloc(sizeof(*n));
	*n = (heap_node_t){x, p};
	return n;
}

static void heap_upswap(heap_node_t *n) {
	void* x = n->data;
	n->data = n->p->data;
	n->p->data = x;
}

struct node_info {heap_node_t *p; heap_node_t **n;};
static struct node_info heap_last_element(heap_t* h) {
	int N = h->N;
	int D = (1<<h->D);
	int lr = 0;
	heap_node_t *n = h->root;
	heap_node_t *p = 0;
	// Figure out where the node is supposed to go.
	while (D > 1) {
		p = n;
		lr = 2*N/D;
		if (lr) {
			n = n->r;
			N -= D>>1;
		} else {
			n = n->l;
		}
		D = D>>1;
	}
	return (struct node_info){p, lr ? &p->r : &p->l};
}

void heap_push(heap_t* h, void* x) {
	heap_node_t *t = heap_node_new(0, x);
	if (!h->D) {
		h->root = t;
		// h->N=0; // Unnecessary if heap_init() was used correctly.
		h->D=1;
		return;
	}
	struct node_info n = heap_last_element(h);
	// Put the node in that spot, and add items to the tree.
	*n.n = t;
	t->p = n.p;
	// Adjust heap accounting.
	h->N++;
	if (h->N == (1<<h->D)) {h->N = 0; h->D++;}
	// Swap up the tree until the heap property is restored.
	while (t->p && h->cmp(t->data, t->p->data) < 0) {
		heap_upswap(t);
		t=t->p;
	}
}

void* heap_pop     (heap_t* h) {
	if (!h->root) {return 0;}
	h->N--;
	if (h->N < 0) {
		h->D--;
		h->N = (1<<h->D)-1;
	}
	struct node_info last = heap_last_element(h);
	void* r;
	if (!last.p) {
		r = h->root->data;
		free(h->root);
		h->root = 0;
		h->D = h->N = 0;
		return r;
	}
	// Save the root value for returning.
	r = h->root->data;
	// Transfer last to the root.
	h->root->data = (*last.n)->data;
	// Unstitch and remove the last element from the heap.
	free(*last.n);
	*last.n = 0;
	heap_node_t *n = h->root, *most;
	while (n) {
		most = n->l;
		if (!most) {most = n->r;}
		else if (n->r) {most = h->cmp(n->r->data, n->l->data)>0 ? n->l : n->r;}
		if (most && h->cmp(n->data, most->data)>0) {heap_upswap(most);}
		n = most;
	}
	return r;
}

void  heap_clean (heap_t* h) {while (h->root) {heap_pop(h);}}

#ifdef UNITHEAP

#include <stdio.h>

static void heap_print_level(heap_node_t* n, int l) {
	if (!n) {printf("N "); return;}
	if (l == 0) {printf("%d ", *(int*)n->data); return;}
	heap_print_level(n->l, l-1);
	heap_print_level(n->r, l-1);
}

static void heap_print(heap_t* h) {
	int l;
	printf("Heap: D=%d N=%d \n", h->D, h->N);
	for (l = 0; l<h->D+1; l++) {
		printf("%d: [ ", l);
		heap_print_level(h->root, l);
		printf("]\n");
	}
	printf("\n\n");
}

int test_cmp(void* x, void* y) {return *(int*)y - *(int*)x;}
int main() {
	int x[31];
	srand(4);
	int i;
	for (i=0; i<sizeof(x)/sizeof(*x); i++) {x[i] = rand();}

	heap_t h = heap_init(test_cmp);
	for (i=0; i<sizeof(x)/sizeof(*x); i++) {
		heap_push(&h, x+i);
	}
	heap_print(&h);
	while (h.root) {
		printf("%d\n", *(int*)heap_pop(&h));
	}
	return 0;
}

#endif

reflist_node_t* reflist_add(reflist_t* ll, void* data) {
	reflist_node_t * const x = malloc(sizeof(*x));
	*x = (reflist_node_t){ll};
	x->data = data;

	if (!ll->f) {ll->f = ll->l = x; return x;}
	x->p = ll->l;
	x->p->n = x;
	ll->l = x;
	return x;
}

reflist_node_t*  reflist_addafter(reflist_node_t* ln, void* data) {
	reflist_t * const      ll = ln->par;
	reflist_node_t * const x  = malloc(sizeof(*x));
	*x = (reflist_node_t){ll};
	x->data = data;

	x->p = ln;
	x->n = ln->n;
	if (x->n) {x->n->p = x;}
	if (x->p) {x->p->n = x;}
	if (ll->l == x->p) {ll->l = x;}
	return x;
}

reflist_node_t*  reflist_addprev (reflist_node_t* ln, void* data) {
	reflist_t * const      ll = ln->par;
	reflist_node_t * const x  = malloc(sizeof(*x));
	*x = (reflist_node_t){ll};
	x->data =data;

	x->n = ln;
	x->p = ln->p;
	if (x->n) {x->n->p = x;}
	if (x->p) {x->p->n = x;}
	if (ll->f == x->n) {ll->f = x;}
	return x;
}

static void* reflist_identity(void* x) {return x;}
reflist_t *reflist_copy(reflist_t* lln, reflist_t* ll, void* (*copy_func)(void*)) {
	if (!copy_func) {copy_func = reflist_identity;}
	*lln = reflist_init();

	reflist_node_t *iter = ll->f;
	while (iter) {
		reflist_add(lln, copy_func(iter->data));
		iter = iter->n;
	}

	return lln;
}

void* reflist_remove (reflist_node_t** hln) {
	reflist_node_t * const ln = *hln;
	reflist_t * const      ll = ln->par;
	reflist_node_t * const  p = ln->p;
	reflist_node_t * const  n = ln->n;
	void* r;

	if (p) {p->n = n;}
	if (n) {n->p = p;}

	if (ll->f == ln) {ll->f = ln->n;}
	if (ll->l == ln) {ll->l = ln->p;}

	r = ln->data;
	free(ln);
	*hln = p ? p : n;
	return r;
}

void reflist_clean (reflist_t* ll, free_func f) {
	reflist_node_t * ln = ll->f;
	if (f)
		while (ln) f(reflist_remove(&ln));
	else 
		while (ln) reflist_remove(&ln);
}

#ifdef UNITREFLIST

#include <stdio.h>

void print_int(void* intp) {
	int* const v = intp;
	printf("%d ", *v);
}

int main() {
	reflist_t ll; reflist_node_t *iter, *i;
	int x[] = {0, 1, 2, 3, 4, 5, 6, 7};

	// Test 1
	ll = reflist_init();
	reflist_add(&ll, x+0);
	reflist_add(&ll, x+1);
	iter = reflist_add(&ll, x+2);
	reflist_add(&ll, x+3);
	reflist_add(&ll, x+4);
	i = ll.f;
	printf("[ "); while (i)
	{
		print_int(i->data);
		i=i->n;
	} printf("]\n");
	printf("Removed: %d\n", *(int*)reflist_remove(&iter));
	printf("[ "); reflist_clean(&ll, print_int); printf("]\n");
	return 0;
}

#endif


void link_addnext(link_t* root, link_t* node) {
	link_t * const n = root->n;
	root->n = node;
	node->p = root;
	node->n = n;
	if (n) {n->p = node;}
}

void link_addprev(link_t* root, link_t* node) {
	link_t * const p = root->n;
	root->p = node;
	node->n = root;
	node->p = p;
	if (p) {p->n = node;}
}

link_t* link_remove(link_t* el) {
	link_t * const p = el->p;
	link_t * const n = el->n;
	el->n = el->p = 0;
	if (p) {p->n = n;}
	if (n) {n->p = p;}
	return p?p:n;
}

void link_add(link_list_t* h, void* el) {
	link_t * const e = el + h->offset;
	if (!h->l) {
		h->l = h->f = e;
		return;
	}
	link_addnext(h->l, e);
	h->l = e;
}

void link_erase(link_list_t* h, void* el) {
	link_t * const e = el + h->offset;
	if (h->l == e) {
		if (h->f == h->l) {h->f = h->l = 0;}
		else {h->l = h->l->p;}
	}
	else if (h->f == e) {h->f = h->f->n;}
	link_remove(e);
}

void link_clean(link_list_t* h, void (*free_func)(void*)) {
	while (h->f) {
		void * const k = (void*)h->f - h->offset;
		link_erase(h, k);
		if (free_func) {free_func(k);}
	}
}

void link_next(link_iter_t* i)
{
	i->node = i->node->n;
	i->el = i->node?(void*)(i->node) - i->offset:0;
}

void link_prev(link_iter_t* i)
{
	i->node = i->node->p;
	i->el = i->node?(void*)(i->node) - i->offset:0;
}
