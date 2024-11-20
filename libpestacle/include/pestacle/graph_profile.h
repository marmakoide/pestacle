#ifndef PESTACLE_GRAPH_PROFILE_H
#define PESTACLE_GRAPH_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL.h>
#include <pestacle/math/average.h>


typedef struct {
	AverageResult time;
} NodeProfile;


extern void
NodeProfile_update(
	NodeProfile* self,
	real_t time_interval
);


struct s_Graph;

typedef struct {
	AverageResult time;
	NodeProfile* node_profiles;
} GraphProfile;


extern void
GraphProfile_init(
	GraphProfile* self,
	struct s_Graph* graph
);


extern void
GraphProfile_destroy(
	GraphProfile* self
);


extern void
GraphProfile_print_report(
	GraphProfile* self,
	struct s_Graph* graph,
	FILE* fp
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_GRAPH_PROFILE_H */ 
