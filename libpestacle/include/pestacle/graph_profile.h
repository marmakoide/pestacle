#ifndef PESTACLE_GRAPH_PROFILE_H
#define PESTACLE_GRAPH_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL.h>


typedef struct {
	float mean_time; // Mean running time
	float m2_time;   // Squared distance from the mean running time
} NodeProfile;


extern void
NodeProfile_update(
	NodeProfile* self,
	float time_interval,
	size_t update_count
);


struct s_Graph;

typedef struct {
	size_t update_count;
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
