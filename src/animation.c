#include <assert.h>
#include "stack.h"
#include "animation.h"
#include "memory.h"


static bool
Animation_check_graph_is_complete(Animation* self) {
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
Animation_topological_sort(Animation* self) {
	bool ret = true;

	Stack stack;
	Stack_init(&stack);

	Dict visited;
	Dict_init(&visited);

	// Get the root node
	const String root_node_instance_name = { "main", 5 };
	Node* root = Animation_get_node_instance(self, &root_node_instance_name);
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
Animation_init(
	Animation* self
) {
	assert(self != 0);

	Dict_init(&(self->node_instance_dict));
	self->sorted_node_count = 0;
	self->sorted_nodes = 0;
}


bool
Animation_setup(
	Animation* self,
	int screen_width,
	int screen_height
) {
	assert(self != 0);

	// Setup the graph
	if (!Animation_topological_sort(self))
		goto failure;

	if (!Animation_check_graph_is_complete(self))
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
		if (!Node_setup(*node_ptr, screen_width, screen_height))
			goto failure;

	// Job done
	return true;

	// Failure handling
failure:
	Animation_destroy(self);
	return false;
}


void
Animation_destroy(
	Animation* self
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
	DictIterator_init(&it, &(self->node_instance_dict));
	for( ; DictIterator_has_next(&it); DictIterator_next(&it)) {
		Node* node = (Node*)it.entry->value;
		Node_destroy(node);
		free(node);
	}

	// Deallocate node dictionary
	Dict_destroy(&(self->node_instance_dict)); 
}


Node*
Animation_get_node_instance(
	Animation* self,
	const String* name
) {
	assert(self != 0);
	assert(name != 0);
	assert(name->data != 0);

	DictEntry* entry =
		Dict_find(&(self->node_instance_dict), name);

	if (!entry)
		return 0;

	return (Node*)entry->value;
}



bool
Animation_add_node_instance(
	Animation* self,
	const String* instance_name,
	const NodeDelegate* delegate
) {
	assert(self != 0);
	assert(instance_name != 0);
	assert(delegate_name != 0);

	// Check that there is no instance with that name already
	DictEntry* entry =
		Dict_find(&(self->node_instance_dict), instance_name);

	if (entry)
		return false;

	// Create the node instance
	Node* node = Node_new(instance_name, delegate);

	// Update the instance dictionary
	Dict_insert(&(self->node_instance_dict), &(node->name))->value = node;
	
	// Job done
	return true;
}


void
Animation_handle_event(
	Animation* self,
	const Event* event
) {
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		Node_handle_event(*node_ptr, event);
}


void
Animation_render(
	const Animation* self,
	SDL_Surface* dst
) {
	SDL_BlitSurface(
		Node_get(self->sorted_nodes[0]).rgb_surface,
		0,
		dst,
		0
	);
}


void
Animation_update(
	Animation* self
) {
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		Node_update(*node_ptr);
}