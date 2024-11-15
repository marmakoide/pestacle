#include <assert.h>
#include <pestacle/stack.h>
#include <pestacle/graph.h>
#include <pestacle/memory.h>


static bool
Graph_check_graph_is_complete(
	Graph* self
) {
	assert(self);
	assert(self->sorted_nodes);

	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		if (!Node_is_complete(*node_ptr)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"node '%s' is not complete\n",
				(*node_ptr)->name
			);
			return false;
		}

	return true;
}


static void
push_node_inputs(
	Stack* stack, 
	Node* node
) {
	Node** input_ptr = node->inputs;
	const NodeInputDefinition* input_def = node->delegate->input_defs;
	for(; input_def->type != NodeType__last; ++input_ptr, ++input_def)
		if (*input_ptr)
			Stack_push(stack, *input_ptr);
}


static void
Scope_gather_node_with_type(
	Scope* self,
	Stack* out,
	enum NodeType node_type
) {
	DictIterator it;

	ScopeMember root = {
		ScopeMemberType__scope,
		{ .scope = self },
		0
	};

	Stack stack;
	Stack_init(&stack);
	
	Stack_push(&stack, &root);
	while(!Stack_empty(&stack)) {
		ScopeMember* member = (ScopeMember*)Stack_pop(&stack);
		switch(member->type) {
			case ScopeMemberType__node:
				if (member->node->delegate->type == node_type)
					Stack_push(out, member->node);
				break;

			case ScopeMemberType__scope:
				DictIterator_init(&it, &(member->scope->members));
				for( ; DictIterator_has_next(&it); DictIterator_next(&it))
					Stack_push(&stack, it.entry->value);
				break;

			default:
				break;
		}
	}

	Stack_destroy(&stack);
}


static bool
Graph_topological_sort(
	Graph* self,
	Stack* root_nodes
) {
	bool ret = true;

	Stack stack;
	Stack_init(&stack);

	Dict visited;
	Dict_init(&visited);

	// Count the size of the component corresponding to the root node
	size_t component_size = 0;

	Stack_copy(&stack, root_nodes);
	while(!Stack_empty(&stack)) {
		Node* node = (Node*)Stack_pop(&stack);

		if (!Dict_find(&visited, node->name)) {
			Dict_insert(&visited, node->name);

			component_size += 1;
			push_node_inputs(&stack, node);
		}
	}

	self->sorted_node_count = component_size;

	// Store the nodes in topological order
	Dict_clear(&visited);

	self->sorted_nodes = (Node**)checked_malloc(component_size * sizeof(Node*));
	Node** sorted_node_ptr = self->sorted_nodes;

	Stack_copy(&stack, root_nodes);
	while(!Stack_empty(&stack)) {
		Node* node = (Node*)Stack_pop(&stack);

		if (!Dict_find(&visited, node->name)) {
			Dict_insert(&visited, node->name);

			*sorted_node_ptr = node;
			sorted_node_ptr += 1;
			push_node_inputs(&stack, node);
		}
	}

	// Job done
	Dict_destroy(&visited);
	Stack_destroy(&stack);
	return ret;
}


bool
Graph_init(
	Graph* self,
	Scope* scope
) {
	assert(self);

	// Initialize members
	self->sorted_node_count = 0;
	self->sorted_nodes = 0;

	// Gather root nodes
	Stack root_nodes;
	Stack_init(&root_nodes);
	Scope_gather_node_with_type(scope, &root_nodes, NodeType__void);

	// Sort the nodes
	if (!Graph_topological_sort(self, &root_nodes))
		goto failure;

	// Check validity
	if (!Graph_check_graph_is_complete(self))
		goto failure;

	// Job done
	Stack_destroy(&root_nodes);
	return true;

failure:
	Stack_destroy(&root_nodes);
	Graph_destroy(self);
	return false;
}


void
Graph_destroy(
	Graph* self
) {
	assert(self);

	if (self->sorted_nodes) {
		free(self->sorted_nodes);

		#ifdef DEBUG
		self->sorted_node_count = 0;
		self->sorted_nodes = 0;
		#endif
	}
}


bool
Graph_setup(
	Graph* self
) {
	assert(self);

	// Setup the nodes in reverse topological order
	Node** node_ptr = self->sorted_nodes + self->sorted_node_count - 1;
	for(size_t i = self->sorted_node_count; i != 0; --i, --node_ptr) {
		Node* node = *node_ptr;

		SDL_Log(
			"setup node %s : %s",
			node->name,
			node->delegate->name
		);

		if (!Node_setup(node)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"node %s : %s setup failure",
				node->name,
				node->delegate->name
			);
			return false;
		}
	}

	// Job done
	return true;
}


void
Graph_update(
	Graph* self
) {
	assert(self);

	// Update the nodes in reverse topological order
	Node** node_ptr = self->sorted_nodes + self->sorted_node_count - 1;
	for(size_t i = self->sorted_node_count; i != 0; --i, --node_ptr)
		Node_update(*node_ptr);
}


void
Graph_update_with_profile(
	Graph* self,
	GraphProfile* profile
) {
	assert(self);
	assert(profile);

	// Update the number of updates
	profile->update_count += 1;

	// Update the nodes in reverse topological order
	Node** node_ptr = self->sorted_nodes + self->sorted_node_count - 1;
	NodeProfile* profile_ptr = profile->node_profiles + self->sorted_node_count - 1;

	for(size_t i = self->sorted_node_count; i != 0; --i, --node_ptr, --profile_ptr) {
		// Update the node
		Uint64 start_time = SDL_GetPerformanceCounter();
		Node_update(*node_ptr);
		Uint64 end_time = SDL_GetPerformanceCounter();

		// Track the running time for that node
		NodeProfile_update(
			profile_ptr,
			((float)(end_time - start_time)) / SDL_GetPerformanceFrequency(),
			profile->update_count
		);
	}
}
