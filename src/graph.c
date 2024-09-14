#include <assert.h>
#include "stack.h"
#include "graph.h"
#include "memory.h"


static bool
Graph_check_graph_is_complete(Graph* self) {
	assert(self != 0);
	assert(self->sorted_nodes != 0);

	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		if (!Node_is_complete(*node_ptr)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"node '%s' is not complete\n",
				(*node_ptr)->name.data
			);
			return false;
		}

	return true;
}


static void
push_node_inputs(Stack* stack, Node* node) {
	Node** input_ptr = node->inputs;
	const NodeInputDefinition* input_def = node->delegate->input_defs;
	for(; input_def->type != NodeType__last; ++input_ptr, ++input_def)
		if (*input_ptr)
			Stack_push(stack, *input_ptr);
}


static void
Domain_gather_node_with_type(
	Domain* self,
	Stack* out,
	enum NodeType node_type
) {
	DictIterator it;

	DomainMember root = {
		DomainMemberType__domain,
		{ .domain = self }
	};

	Stack stack;
	Stack_init(&stack);
	
	Stack_push(&stack, &root);
	while(!Stack_empty(&stack)) {
		DomainMember* member = (DomainMember*)Stack_pop(&stack);
		switch(member->type) {
			case DomainMemberType__node:
				if (member->node->delegate->type == node_type)
					Stack_push(out, member->node);
				break;

			case DomainMemberType__domain:
				DictIterator_init(&it, &(member->domain->members));
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

		if (!Dict_find(&visited, &(node->name))) {
			Dict_insert(&visited, &(node->name));

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

		if (!Dict_find(&visited, &(node->name))) {
			Dict_insert(&visited, &(node->name));

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
	Domain* domain
) {
	assert(self != 0);

	// Initialize members
	self->sorted_node_count = 0;
	self->sorted_nodes = 0;

	// Gather root nodes
	Stack root_nodes;
	Stack_init(&root_nodes);
	Domain_gather_node_with_type(domain, &root_nodes, NodeType__void);

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
	Node** node_ptr = self->sorted_nodes + self->sorted_node_count - 1;
	for(size_t i = self->sorted_node_count; i != 0; --i, --node_ptr)
		if (!Node_setup(*node_ptr))
			return false;

	return true;
}


void
Graph_handle_event(
	Graph* self,
	const Event* event
) {
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		Node_handle_event(*node_ptr, event);
}


void
Graph_update(
	Graph* self
) {
	// Update the nodes in topological order
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		Node_update(*node_ptr);
}