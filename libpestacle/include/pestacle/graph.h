#ifndef PESTACLE_GRAPH_H
#define PESTACLE_GRAPH_H

#ifdef __cplusplus
extern "C" {
#endif


#include <SDL.h>
#include <pestacle/scope.h>
#include <pestacle/graph_profile.h>
#include <pestacle/math/vector.h>
#include <pestacle/math/matrix.h>


struct s_Graph {
	size_t sorted_node_count;
	Node** sorted_nodes;
}; // struct s_Graph

typedef struct s_Graph Graph;


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
Graph_update(
	Graph* self
);


extern void
Graph_update_with_profile(
	Graph* self,
	GraphProfile* profile
);



#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_GRAPH_H */ 
