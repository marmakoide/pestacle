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

	Node** node_ptr;
	NodeProfile* profile_ptr;


	// Compute the max length for the node identification field
	size_t node_id_max_len = 0;

	node_ptr = graph->sorted_nodes;
	profile_ptr = self->node_profiles;
	for(size_t i = graph->sorted_node_count; i != 0; --i, ++node_ptr, ++profile_ptr) {
		size_t node_id_len =
			(*node_ptr)->name.len +
			(*node_ptr)->delegate->name.len;

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
			(*node_ptr)->name.data,
			(*node_ptr)->delegate->name.data
		);

		size_t node_id_len =
			(*node_ptr)->name.len +
			(*node_ptr)->delegate->name.len;

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
