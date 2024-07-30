#ifndef PESTACLE_GRAPH_H
#define PESTACLE_GRAPH_H

#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "node.h"
#include "dict.h"


typedef struct {
	Dict node_dict;
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
 * Return the node with a given name, or 0 if no such node exists
 */

extern Node*
Graph_get_node(
	Graph* self,
	const String* name
);


/*
 * Add a new node
 *   self : the graph to which we add a node
 *   name : name of the node
 *   delegate : delegate for the node
 *
 * Returns false if a node with the same name already exists
 */

extern bool
Graph_add_node(
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