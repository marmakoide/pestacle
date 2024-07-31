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


static bool
Graph_topological_sort(Graph* self) {
	bool ret = true;

	Stack stack;
	Stack_init(&stack);

	Dict visited;
	Dict_init(&visited);

	// Get the root node
	const String root_node_name = { "main", 5 };
	Node* root = Graph_get_node(self, &root_node_name);
	if (!root) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"no 'main' node defined\n"
		);
		ret = false;
		goto termination;
	}

	// Count the size of the component corresponding to the root node
	size_t component_size = 0;

	Stack_push(&stack, root);
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

	Stack_push(&stack, root);
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
termination:
	Dict_destroy(&visited);
	Stack_destroy(&stack);
	return ret;
}


void
Graph_init(
	Graph* self
) {
	assert(self != 0);

	Dict_init(&(self->node_dict));
	self->sorted_node_count = 0;
	self->sorted_nodes = 0;
}


void
Graph_destroy(
	Graph* self
) {
	// Deallocate sorted node dictionary
	if (self->sorted_nodes) {
		free(self->sorted_nodes);

		#ifdef DEBUG
		self->sorted_node_count = 0;
		self->sorted_nodes = 0;
		#endif
	}

	// Deallocate all nodes
	DictIterator it;
	DictIterator_init(&it, &(self->node_dict));
	for( ; DictIterator_has_next(&it); DictIterator_next(&it)) {
		Node* node = (Node*)it.entry->value;
		Node_destroy(node);
		free(node);
	}

	// Deallocate node dictionary
	Dict_destroy(&(self->node_dict)); 
}


Node*
Graph_get_node(
	Graph* self,
	const String* name
) {
	assert(self != 0);
	assert(name != 0);
	assert(name->data != 0);

	DictEntry* entry = Dict_find(&(self->node_dict), name);
	if (!entry)
		return 0;

	return (Node*)entry->value;
}



bool
Graph_add_node(
	Graph* self,
	const String* name,
	const NodeDelegate* delegate
) {
	assert(self != 0);
	assert(name != 0);
	assert(delegate != 0);

	// Check that there is no node with that name already
	if (!Dict_find(&(self->node_dict), name)) {
		// Create and add a new node
		Node* node = Node_new(name, delegate);
		Dict_insert(&(self->node_dict), &(node->name))->value = node;
		return true;
	}

	// Job done
	return false;
}


bool
Graph_setup(
	Graph* self
) {
	assert(self != 0);

	// Setup the graph
	if (!Graph_topological_sort(self))
		goto failure;

	if (!Graph_check_graph_is_complete(self))
		goto failure;

	// Check that the root node have proper type
	if (self->sorted_nodes[0]->delegate->type != NodeType__rgb_surface) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"'main' node output type is not rgb_surface\n"
		);
		goto failure;
	}

	// Setup the nodes
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		if (!Node_setup(*node_ptr))
			goto failure;

	// Job done
	return true;

	// Failure handling
failure:
	Graph_destroy(self);
	return false;
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


SDL_Surface*
Graph_output(
	const Graph* self
) {
	return Node_output(self->sorted_nodes[0]).rgb_surface;
}


void
Graph_update(
	Graph* self
) {
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		Node_update(*node_ptr);
}