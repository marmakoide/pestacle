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

	AverageResult_init(&(self->time));
}


void
NodeProfile_update(
	NodeProfile* self,
	real_t time_interval
) {
	assert(self);
	assert(time_interval >= 0);
	assert(!isnan(time_interval));

	AverageResult_accumulate(&(self->time), time_interval);
}


// --- GraphProfile -----------------------------------------------------------

void
GraphProfile_init(
	GraphProfile* self,
	struct s_Graph* graph
) {
	assert(self);
	assert(graph);

	AverageResult_init(&(self->time));

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
	assert(graph);
	assert(fp);

	Node** node_ptr;
	NodeProfile* profile_ptr;

	StringList str_list;
	StringList_init(&str_list);

	// Compute the max length for the node identification field
	size_t node_id_max_len = 0;

	node_ptr = graph->sorted_nodes;
	profile_ptr = self->node_profiles;
	for(size_t i = graph->sorted_node_count; i != 0; --i, ++node_ptr, ++profile_ptr) {
		StringList_clear(&str_list);
		Node_get_delegate_path(*node_ptr, &str_list);

		size_t node_id_len =
			strlen((*node_ptr)->name) +
			StringList_content_length(&str_list) +
			StringList_length(&str_list);

		if (node_id_max_len < node_id_len)
			node_id_max_len = node_id_len;
	}

	// Print the report
	fprintf(fp, "%zu updates\n", AverageResult_count(&self->time));

	node_ptr = graph->sorted_nodes;
	profile_ptr = self->node_profiles;
	for(size_t i = graph->sorted_node_count; i != 0; --i, ++node_ptr, ++profile_ptr) {
		StringList_clear(&str_list);
		Node_get_delegate_path(*node_ptr, &str_list);
		StringList_reverse(&str_list);
		
		fprintf(
			fp,
			"  %s : ",
			(*node_ptr)->name
		);

		StringList_print(&str_list, fp);

		size_t node_id_len =
			strlen((*node_ptr)->name) +
			StringList_content_length(&str_list) +
			StringList_length(&str_list);

		for(size_t j = 0; j < node_id_max_len - node_id_len; ++j)
			fputc(' ', fp);

		fprintf(
			fp,
			" => %.3f msec (+/- %.3f)\n",
			1e3f * AverageResult_mean(&(profile_ptr->time)),
			3 * 1e3f * AverageResult_stddev(&(profile_ptr->time))
		);
	}

	fprintf(
		fp,
		"total %.3f msec (+/- %.3f)\n",
		1e3f * AverageResult_mean(&(self->time)),
		3 * 1e3f * AverageResult_stddev(&(self->time))
	);

	// Release ressources
	StringList_destroy(&str_list);
}
