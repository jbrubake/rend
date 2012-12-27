#include <stddef.h>
#include "entities.h"
#include "memwatch.h"
#include "debug.h"

#define _ENTITIES_C_

typedef struct entity_s {
	NEDTRIE_ENTRY(entity_s) manager_link;
	entity_id ent_i;
	link_list_t components;
} entity_t;
typedef struct entity_container_s entity_container_t;
NEDTRIE_HEAD(entity_container_s, entity_s);
static entity_container_t entities;

size_t componentkey(const component_t *r) {return (size_t)r->ent_i;}
size_t entitykey(const entity_t *r) {return (size_t)r->ent_i;}

NEDTRIE_GENERATE(static, component_container_s, component_s, manager_link, componentkey,
  NEDTRIE_NOBBLEZEROS(component_container_s))
NEDTRIE_GENERATE(static, entity_container_s, entity_s, manager_link, entitykey,
  NEDTRIE_NOBBLEZEROS(entity_container_s))

static inline entity_t * entity_key(entity_id * i) {return (void*)(i) - offsetof(entity_t, ent_i);}
static inline component_t * component_key(entity_id * i) {return (void*)(i) - offsetof(component_t, ent_i);}

static manager_t * managers[CPT_NUM];

// Private:
//#define REMOVE_RELATIONSHIPS(...)
static inline void relationship_clean(entity_id s_id, component_type s_type) {
    relationship_t *last = 0, *r;
    relationship_iter ri;
    int i; for (i=CPT_ENTITY; i<CPT_END; i++) {
        ri = relationship_findparents(s_id, s_type, i);
        last = relationship_next(&ri);
        while (last) {
            r = relationship_next(&ri);
            relationship_destroy(last);
            last = r;
        }
        ri = relationship_findchildren(s_id, s_type, i);
        last = relationship_next(&ri);
        while (last) {
            r = relationship_next(&ri);
            relationship_destroy(last);
            last = r;
        }
    }
}

static inline void component_free(component_t * c) {
    relationship_clean(c->ent_i, c->type);
	manager_t * const mgr = managers[c->type-CPT_FIRST];
	NEDTRIE_REMOVE(component_container_s, &mgr->components, c);
	free(c);
}
static void _component_free(void * c) {component_free(c);}

void inline entity_free(entity_t * ent) {
// WORKING
    relationship_clean(ent->ent_i, CPT_ENTITY);
	link_clean(&ent->components, _component_free); // Remove entity components.
    NEDTRIE_REMOVE(entity_container_s, &entities, ent); // Remove the entity.
    free(ent);
}
//static void _entity_free(void * c) {entity_free(c);}

// Public:

#include "bear.h"

entity_id entity_create() {
    // FIXME: This section stops working with -Olevel

    static entity_id last_id = 0;
    entity_t * r = 0;
    do {
        last_id++;
		if (last_id >= 1<<20) {last_id = 0;}
        r = NEDTRIE_FIND(entity_container_s, &entities, entity_key(&last_id));
    } while (r);
    r = malloc(sizeof(*r));
    r->ent_i = last_id;
	r->components = link_init(component_t, entity_link);
    NEDTRIE_INSERT(entity_container_s, &entities, r);
    return r->ent_i;
}

void entity_destroy(entity_id ent_i) {
    entity_t * r = NEDTRIE_FIND(entity_container_s, &entities, entity_key(&ent_i));
    if (!r) {return;}
    entity_free(r);
}

void * component_attach(entity_id ent_i, component_type cpt_t) {
	component_t * cpt = NEDTRIE_FIND(component_container_s, &managers[cpt_t - CPT_FIRST]->components, component_key(&ent_i));
	if (cpt) {return cpt;}

	manager_t * const mgr = managers[cpt_t - CPT_FIRST];
	entity_t  * const ent = NEDTRIE_FIND(entity_container_s, &entities, entity_key(&ent_i));
	cpt = malloc(mgr->size);
	cpt->type = cpt_t;
	cpt->ent_i = ent_i;
	cpt->entity_link = (link_t){0};
	if (mgr->cpt_create) {mgr->cpt_create(cpt);}
	NEDTRIE_INSERT(component_container_s, &mgr->components, cpt);
	link_add(&ent->components, cpt);
	return cpt;
}

void component_remove(void * cptv) {
	component_t * const cpt = cptv;
	if (!cpt) {return;}
	manager_t * const mgr = managers[cpt->type - CPT_FIRST];
	if (mgr->cpt_destroy) {mgr->cpt_destroy(cpt);}
	component_free(cpt);
}

void * component_get (entity_id ent_i, component_type type) {
	return NEDTRIE_FIND(component_container_s, &managers[type-CPT_FIRST]->components, component_key(&ent_i));
}

// Define the index of relationships by child.
typedef struct bychild_s bychild_t;
NEDTRIE_HEAD(bychild_s, relationship_s);
static bychild_t bychild_index;
static size_t childkey(const relationship_t *r) {return (size_t)((r->child<<12) + (r->child_t << 6) + r->parent_t);}
NEDTRIE_GENERATE(static, bychild_s, relationship_s, child_link, childkey,
  NEDTRIE_NOBBLEZEROS(bychild_s))

// Define the index of relationships by parent.
typedef struct byparent_s byparent_t;
NEDTRIE_HEAD(byparent_s, relationship_s);
static byparent_t byparent_index;
static size_t parentkey(const relationship_t *r) {return (size_t)((r->parent<<12) + (r->parent_t << 6) + r->child_t);}
NEDTRIE_GENERATE(static, byparent_s, relationship_s, parent_link, parentkey,
  NEDTRIE_NOBBLEZEROS(byparent_s))

// Creates a relationship between the entity/component
// parent and the entity/component child.
relationship_t * relationship_create(entity_id parent, component_type parent_t,
									 entity_id child,  component_type child_t) {
	relationship_t * r = malloc(sizeof(*r));
	r->parent = parent;
	r->child = child;
	r->parent_t = parent_t;
	r->child_t = child_t;

	NEDTRIE_INSERT(bychild_s,  &bychild_index, r);
	NEDTRIE_INSERT(byparent_s, &byparent_index, r);

	return r;
}
// Destroys a given relationship.
void relationship_destroy(relationship_t * r) {
    if (!r) {return;}
	NEDTRIE_REMOVE(bychild_s,  &bychild_index, r);
	NEDTRIE_REMOVE(byparent_s, &byparent_index, r);

	free(r);
}

static inline relationship_iter relationship_find(entity_id related, int same_type, int other_type, int search_by) {
    relationship_t r = {};
	relationship_iter ri;
	ri.parent = search_by;
    r.related[search_by] = related;
    r.related_type[!search_by] = same_type;
	r.related_type[search_by] = other_type;
    relationship_t * t;
    if (ri.parent) {ri.cur = NEDTRIE_FIND(bychild_s,  &bychild_index,  &r);  while ( ri.cur && (t = NEDTRIE_PREVLEAF(bychild_s,   ri.cur)) ) {ri.cur = t;} }
    else           {ri.cur = NEDTRIE_FIND(byparent_s, &byparent_index, &r);  while ( ri.cur && (t = NEDTRIE_PREVLEAF(byparent_s,  ri.cur)) ) {ri.cur = t;} }
	return ri;
}

// Finds all relationships in which the parent is entity_id.
relationship_iter relationship_findchildren(entity_id parent_id, int parent_type, int child_type)  {return relationship_find(parent_id, parent_type, child_type,  0);}
relationship_iter relationship_findparents (entity_id child_id,  int child_type,  int parent_type) {return relationship_find(child_id,  child_type,  parent_type, 1);}
relationship_t * relationship_next(relationship_iter* ri) {
    relationship_t * ret = ri->cur;
    if (!ret) {return ret;}
    if (!ri->parent) {
        // Searching by parent
        ri->cur = NEDTRIE_NEXTLEAF(byparent_s, ri->cur);
    }
    else {
        // Searching by child
        ri->cur = NEDTRIE_NEXTLEAF(bychild_s, ri->cur);
    }
    return ret;
}

relationship_t * relationship_related(entity_id parent_id, int parent_type, entity_id child_id, int child_type) {
    relationship_t * r;
    relationship_iter ri = relationship_findparents(child_id, child_type, parent_type);
    while ((r = relationship_next(&ri))) {
        if (r->parent == parent_id) {return r;}
    }
    return 0; // No result
}

void entity_init() {
    NEDTRIE_INIT(&entities);
	NEDTRIE_INIT(&bychild_index);
	NEDTRIE_INIT(&byparent_index);
	int i;

#define MANAGER(man) \
managers[(man).type-CPT_FIRST] = &(man); \
NEDTRIE_INIT(&managers[(man).type-CPT_FIRST]->components);

	// Add new managers here.
	MANAGER(cpt_name);
	MANAGER(cpt_hair);
	MANAGER(cpt_colour);

#undef MANAGER

	// Activate each manager's custom code.
	for (i = 0; i<CPT_NUM; i++) {if (managers[i]->cpt_init) {managers[i]->cpt_init();}}
}

void entity_cleanall() {
	entity_t *r;
	// I want a function that will keep giving me elements in any order.
	// Unfortunately, the best option is NEDTRIE_MIN at the moment.
	// NEDTRIE_FOREACH and NEDTRIE_FOREACH_SAFE are both terrible.
	while ( (r = NEDTRIE_MIN(entity_container_s, &entities)) ) {entity_free(r);}
}
