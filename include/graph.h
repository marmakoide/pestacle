#ifndef PESTACLE_GRAPH_H
#define PESTACLE_GRAPH_H

#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "node.h"
#include "dict.h"


typedef struct {
	Dict node_instance_dict;
	size_t sorted_node_count;
	Node** sorted_nodes;
} Graph;


extern void
Graph_init(
	Graph* self
);


extern bool
Graph_setup(
	Graph* self,
	int screen_width,
	int screen_height
);


extern void
Graph_destroy(
	Graph* self
);


/*
 * Return the node instance with a given name, or 0 if no such instance exists
 */

extern Node*
Graph_get_node_instance(
	Graph* self,
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
Graph_add_node_instance(
	Graph* self,
	const String* name,
	const NodeDelegate* delegate
);


extern void
Graph_handle_event(
	Graph* self,
	const Event* event
);


extern SDL_Surface*
Graph_output(
	const Graph* self
);


extern void
Graph_update(
	Graph* self
);


#endif /* PESTACLE_GRAPH_H */ 