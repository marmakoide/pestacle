#ifndef PESTACLE_ANIMATION_H
#define PESTACLE_ANIMATION_H

#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "node.h"
#include "dict.h"


typedef struct {
	Dict node_instance_dict;
	size_t sorted_node_count;
	Node** sorted_nodes;
} Animation;


extern void
Animation_init(
	Animation* self
);


extern bool
Animation_setup(
	Animation* self,
	int screen_width,
	int screen_height
);


extern void
Animation_destroy(
	Animation* self
);


/*
 * Return the node instance with a given name, or 0 if no such instance exists
 */

extern Node*
Animation_get_node_instance(
	Animation* self,
	const String* name
);


/*
 * Add a new node instance
 *   self : the graph to which we add a node instance
 *   name : name of the node instance
 *   delegate : delegate for the node instance
 *
 * Returns false if a node instance with the same name already exists
 */

extern bool
Animation_add_node_instance(
	Animation* self,
	const String* name,
	const NodeDelegate* delegate
);


extern void
Animation_handle_event(
	Animation* self,
	const Event* event
);


extern SDL_Surface*
Animation_output(
	const Animation* self
);


extern void
Animation_update(
	Animation* self
);


#endif /* PESTACLE_ANIMATION_H */ 