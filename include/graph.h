#ifndef PESTACLE_GRAPH_H
#define PESTACLE_GRAPH_H

#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "domain.h"

typedef struct {
	size_t sorted_node_count;
	Node** sorted_nodes;
} Graph;


extern bool
Graph_init(
	Graph* self,
	Domain* domain
);


extern void
Graph_destroy(
	Graph* self
);


extern bool
Graph_setup(
	Graph* self
);


extern void
Graph_handle_event(
	Graph* self,
	const Event* event
);


extern void
Graph_update(
	Graph* self
);


#endif /* PESTACLE_GRAPH_H */ 