#include <assert.h>
#include <memory.h>
#include "graph.h"
#include "graph_profile.h"


// --- NodeProfile ------------------------------------------------------------

static void
NodeProfile_init(
	NodeProfile* self
) {
	assert(self != 0);

	self->mean_time = 0;
	self->m2_time = 0;
}


void
NodeProfile_update(
	NodeProfile* self,
	float time_interval,
	size_t update_count
) {
	assert(self ! = 0);
	assert(time_interval >= 0);

	// Welford's online algorithm
    float delta = time_interval - self->mean_time;
    self->mean_time += delta / update_count;
    float delta2 = time_interval - self->mean_time;
    self->m2_time += delta * delta2;
}


// --- GraphProfile -----------------------------------------------------------

void
GraphProfile_init(
	GraphProfile* self,
	struct s_Graph* graph
) {
	assert(self != 0);
	assert(graph != 0);

	self->update_count = 0;
	self->node_profiles = checked_malloc(graph->sorted_node_count * sizeof(NodeProfile));

	NodeProfile* profile_ptr = self->node_profiles;
	for(size_t i = graph->sorted_node_count; i != 0; --i, ++profile_ptr)
		NodeProfile_init(profile_ptr);
}


void
GraphProfile_destroy(
	GraphProfile* self
) {
	assert(self != 0);

	free(self->node_profiles);
}


void
GraphProfile_print_report(
	GraphProfile* self,
	struct s_Graph* graph,
	FILE* fp
) {
	assert(self != 0);
	assert(fp != 0);

	Node** node_ptr = graph->sorted_nodes;
	NodeProfile* profile_ptr = self->node_profiles;

	printf("%lu updates\n", self->update_count);

	for(size_t i = graph->sorted_node_count; i != 0; --i, ++node_ptr, ++profile_ptr) {
		fprintf(
			fp,
			"  %s : %s => %f msec\n",
			(*node_ptr)->name.data,
			(*node_ptr)->delegate->name.data,
			1e3f * profile_ptr->mean_time
		);
	}
}
