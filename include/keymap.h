#ifndef _HASH_H_
#define _HASH_H_

#define KEYHASH_SIZE 256

typedef int (*keyfunc_t)(uint key);

typedef struct keyhashnode_t {
	uint k; // Key value
	keyfunc_t kf; // Key function
	struct keyhashnode_t* n; // Next key
} keyhashnode_t;

typedef struct keyhash_t {
	keyhashnode_t* fn[KEYHASH_SIZE];
} keyhash_t;

void key_add  (keyhashnode_t *k);
int  key_exec (uint k);

// Implementation

keyhash_t keyhash = {{0}};

#define KEY_DEF(k, kf) (keyhashnode_t){k, kf, 0}

void key_add (keyhashnode_t *k) {
	const int v = k->k%KEYHASH_SIZE;    // Get the list the key is stored in.
	DEBUG_INFO("Storing: %d in %d", k->k, v)
	keyhashnode_t** n = &keyhash.fn[v]; // Get the first element of that list.
	while (*n) {n = &(*n)->n;}          // Go to the end of the list.
	*n = k;
}

int key_exec (uint k) {
	const int v = k%KEYHASH_SIZE;          // Get the list the key is stored in.
	keyhashnode_t* n = n = keyhash.fn[v];  // Get the first element of the list.
	while (n && n->k != k) {n = n->n;}     // Search for the key in the list.
	if (n) {return n->kf(k);}
	return 0;
}

#endif
