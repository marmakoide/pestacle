#include <math.h>
#include <assert.h>
#include <pestacle/memory.h>
#include <pestacle/graph.h>
#include <pestacle/graph_profile.h>


// --- NodeProfile ------------------------------------------------------------

static void
NodeProfile_init(
	NodeProfile* self
) {
	assert(self);

	self->mean_time = 0;
	self->m2_time = 0;
}


void
NodeProfile_update(
	NodeProfile* self,
	float time_interval,
	size_t update_count
) {
	assert(self);
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
	assert(self);
	assert(graph);

	self->update_count = 0;
	self->node_profiles =
		(NodeProfile*)checked_malloc(graph->sorted_node_count * sizeof(NodeProfile));

	NodeProfile* profile_ptr = self->node_profiles;
	for(size_t i = graph->sorted_node_count; i != 0; --i, ++profile_ptr)
		NodeProfile_init(profile_ptr);
}


void
GraphProfile_destroy(
	GraphProfile* self
) {
	assert(self);

	free(self->node_profiles);
}


void
GraphProfile_print_report(
	GraphProfile* self,
	struct s_Graph* graph,
	FILE* fp
) {
	assert(self);
	assert(fp);

	Node** node_ptr;
	NodeProfile* profile_ptr;


	// Compute the max length for the node identification field
	size_t node_id_max_len = 0;

	node_ptr = graph->sorted_nodes;
	profile_ptr = self->node_profiles;
	for(size_t i = graph->sorted_node_count; i != 0; --i, ++node_ptr, ++profile_ptr) {
		size_t node_id_len =
			strlen((*node_ptr)->name) +
			strlen((*node_ptr)->delegate->name);

		if (node_id_max_len < node_id_len)
			node_id_max_len = node_id_len;
	}

	// Print the report
	fprintf(fp, "%zu updates\n", self->update_count);

	node_ptr = graph->sorted_nodes;
	profile_ptr = self->node_profiles;
	for(size_t i = graph->sorted_node_count; i != 0; --i, ++node_ptr, ++profile_ptr) {
		fprintf(
			fp,
			"  %s : %s",
			(*node_ptr)->name,
			(*node_ptr)->delegate->name
		);

		size_t node_id_len =
			strlen((*node_ptr)->name) +
			strlen((*node_ptr)->delegate->name);

		for(size_t j = 0; j < node_id_max_len - node_id_len; ++j)
			fputc(' ', fp);

		fprintf(
			fp,
			" => %.3f msec (+/- %.3f)\n",
			1e3f * profile_ptr->mean_time,
			sqrtf(3 * 1e3f * profile_ptr->m2_time / self->update_count)
		);
	}
}
