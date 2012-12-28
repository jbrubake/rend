#include "game.h"

#ifndef _ENTITIES_H_
#define _ENTITIES_H_

// Component manager:

typedef uint entity_id, component_id;
typedef uchar component_type;

enum /* Component types */ {
	CPT_ENTITY,
	CPT_FIRST,
    CPT_NAME = CPT_FIRST,
    CPT_POS,
    CPT_SYMBOL,
    CPT_PART,
    CPT_SOUL,
    CPT_BODY,
    CPT_CREATURE,
    CPT_GENERIC_END,
    CPT_END = CPT_GENERIC_END,
	CPT_NUM = CPT_END - CPT_FIRST,
	// The number of component types that there are.
};

// All components derive from this class.
typedef struct component_s {
    NEDTRIE_ENTRY(component_s) manager_link;
    link_t entity_link;
    component_type type; // The manager type.
    entity_id ent_i; // The parent entity.
} component_t;

// Define a component container
#include "nedtrie.h"
typedef struct component_container_s component_container_t;
NEDTRIE_HEAD(component_container_s, component_s);

// Define a component manager.
typedef struct manager_s {
	component_type type;
	size_t size; // The size of each managed component.
	component_container_t components; // List of instances.
	void (*cpt_init)(void); // Special code called at game init.
	void (*cpt_create)(component_t * x); // Called when a component is created.
//	void (*cpt_update)(component_t * x); // Not necessary?
	void (*cpt_destroy)(component_t * x); // Called when a component is destroyed.
} manager_t;

// Initializes the entity and component managers.
void      entity_init();
// Frees all memory allocated by the component and entity managers.
void      entity_cleanall();

// Create an entity.
entity_id entity_create();
// Destroys the entity, and removes all its components.
void      entity_destroy(entity_id ent_i);

// Attach a component to an entity
void * component_attach(entity_id ent_i, component_type cpt_t);
// Remove a component from its host entity.
// Cleans all relationships.
void   component_remove(void * cpt);
// Retrieves the component by entity id and type.
void * component_get (entity_id ent_i, component_type type);

// A struct describing a parent-child relationship.
enum {
    REL_PARENT,
    REL_CHILD
};
typedef struct relationship_s {
    NEDTRIE_ENTRY(relationship_s) child_link; // Child index link
    NEDTRIE_ENTRY(relationship_s) parent_link; // Parent index link
	union {
		struct {entity_id parent; entity_id child;};
		entity_id related[2];
	};
	union {
		struct {component_type child_t; component_type parent_t;};
		component_type related_type[2];
	};
} relationship_t;

// An iterator returned by the relationship_find*() functions.
typedef struct relationship_iter {
    relationship_t * cur;
    unsigned parent:1; // We are iterating the parent tree.
} relationship_iter;

enum {
    REL_ALL = -3,
    REL_ENTITIES = -2,
    REL_COMPONENTS = -1,
    // Real component types start here.
};

// Creates a relationship between the entity/component
// parent and the entity/component child.
relationship_t * relationship_create(
    entity_id parent,
    component_type parent_t,
    entity_id child,
    component_type child_t
    );
// Destroys a given relationship.
void relationship_destroy(relationship_t * r);

// Finds all relationships in which the parent is entity_id.
relationship_iter relationship_findchildren(entity_id parent_id, int parent_type, int child_type);
relationship_iter relationship_findparents (entity_id child_id,  int child_type,  int parent_type);
relationship_t *  relationship_next(relationship_iter* ri);
relationship_t *  relationship_related(entity_id parent_id, int parent_type, entity_id child_id, int child_type);

/* Entity lists. Use as follows:
    entity_l list = 0; // An empty list.
    entity_add(&list, 12); // Adds entity 12 to the list.
    entity_l iterator = list; // Lists and interators are the same type.
    while (iterator) {
        do_something(iterator->el); // Manipulate the entity element
        iterator = iterator->n;     // Go to the next entity in the list
    }
    ...
    // Remove an element at iterator position (makes sure to remove the first node of the parent list in case of deleting the first node).
    entity_delsafe(list, iterator);
    ...
    // Free the list
    while(list) {entity_del(&list);}
*/

typedef struct entity_l {
    entity_id el;
    struct entity_l * n;
    struct entity_l * p;
} *entity_l;

void     entity_add(entity_l * h, entity_id el);
void     entity_del(entity_l * h);
#define  entity_delsafe(li, it) entity_del( (it == li) ? (it = li->n), &li : &it );
entity_l entity_lcopy(entity_l * h);

#endif
