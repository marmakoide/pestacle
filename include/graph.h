#ifndef PESTACLE_GRAPH_H
#define PESTACLE_GRAPH_H

#ifdef __cplusplus
extern "C" {
#endif


#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "scope.h"

typedef struct {
	size_t sorted_node_count;
	Node** sorted_nodes;
} Graph;


extern bool
Graph_init(
	Graph* self,
	Scope* scope
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


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_GRAPH_H */ 