#include <assert.h>
#include "stack.h"
#include "image.h"
#include "animation.h"
#include "memory.h"

#include "renderers/gradient.h"
#include "renderers/linear_blend.h"


static int
animation_topological_sort(Animation* self) {
	Stack stack;
	stack_init(&stack);

	Dict visited;
	Dict_init(&visited);

	// Get the root node
	const String root_node_instance_name = { "main", 5 };
	Node* root = animation_get_node_instance(self, &root_node_instance_name);
	if (!root)
		return 0;

	// Count the size of the component corresponding to the root node
	size_t component_size = 0;

	stack_push(&stack, root);
	while(!stack_empty(&stack)) {
		Node* node = (Node*)stack_pop(&stack);

		if (!Dict_find(&visited, &(node->name))) {
			Dict_insert(&visited, &(node->name));

			component_size += 1;

			Node** input_node_ptr = node->inputs;
			for(size_t i = node->delegate->input_count; i != 0; --i, ++input_node_ptr)
				if (input_node_ptr)
					stack_push(&stack, *input_node_ptr);
		}
	}


	self->sorted_node_count = component_size;

	// Store the nodes in topological order
	Dict_clear(&visited);

	self->sorted_nodes = (Node**)checked_malloc(component_size * sizeof(Node*));
	Node** sorted_node_ptr = self->sorted_nodes;

	stack_push(&stack, root);
	while(!stack_empty(&stack)) {
		Node* node = (Node*)stack_pop(&stack);

		if (!Dict_find(&visited, &(node->name))) {
			Dict_insert(&visited, &(node->name));

			*sorted_node_ptr = node;
			sorted_node_ptr += 1;

			Node** input_node_ptr = node->inputs;
			for(size_t i = node->delegate->input_count; i != 0; --i, ++input_node_ptr)
				if (input_node_ptr)
					stack_push(&stack, *input_node_ptr);
		}
	}

	// Job done
	Dict_destroy(&visited);
	stack_destroy(&stack);
	return 1;
}


static int
animation_build(Animation* self) {
	const String node_instance_a_name = { "input", 6 };
	const String node_a_name = { "mouse-motion", 13 };

	const String node_instance_b_name = { "main", 5 };
	const String node_b_name = { "heat-diffusion", 15 };

	// Create nodes
	if (!animation_add_node_instance(self, &node_instance_a_name, &node_a_name))
		return 0;

	if (!animation_add_node_instance(self, &node_instance_b_name, &node_b_name))
		return 0;

	Node* node_a = animation_get_node_instance(self, &node_instance_a_name);
	if (!node_a)
		return 0;

	Node* node_b = animation_get_node_instance(self, &node_instance_b_name);
	if (!node_b)
		return 0;

	// Link nodes
	const String slot_name = { "input", 6 };
	if (!node_set_input_slot_by_name(node_b, &slot_name, node_a))
		return 0;

	// Setup parameters
	const String param_name = { "value", 6 };
	NodeParameter* param = node_get_parameter_by_name(node_a, &param_name);
	if (!param)
		return 0;

	param->value = (real_t)32;

	// Job done
	return 1;
}


int
animation_init(
	Animation* self,
	int screen_width,
	int screen_height
) {
	Dict_init(&(self->node_instance_dict));
	self->sorted_node_count = 0;
	self->sorted_nodes = 0;

	self->renderer = 0;

	//
	if (!animation_build(self))
		goto failure;

	if (!animation_topological_sort(self))
		goto failure;

	// Setup the nodes
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		if (!node_setup(*node_ptr, screen_width, screen_height))
			goto failure;

	// Setup the renderer
	//self->renderer = gradient_renderer_new();

	self->renderer =
		linear_blend_renderer_new(
			"assets/paysage-vignoble_200x100.png",
			"assets/soif_200x100.png"
		);

	if (!renderer_setup(self->renderer, screen_width, screen_height))
		goto failure;

	// Job done
	return 1;

	// Failure handling
failure:
	animation_destroy(self);
	return 0;
}


void
animation_destroy(
	Animation* self
) {
	if (self->sorted_nodes) {
		// Deallocate all nodes
		Node** node_ptr = self->sorted_nodes;
		for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr) {
			node_destroy(*node_ptr);
			free(*node_ptr);
		}
		free(self->sorted_nodes);

		#ifdef DEBUG
		self->sorted_node_count = 0;
		self->sorted_nodes = 0;
		#endif
	}

	if (self->renderer) {
		renderer_destroy(self->renderer);
		free(self->renderer);
	}

	Dict_destroy(&(self->node_instance_dict));

	#ifdef DEBUG
	self->sorted_nodes = 0;
	self->renderer = 0;
	#endif
}


Node*
animation_get_node_instance(
	Animation* self,
	const String* instance_name
) {
	assert(self != 0);
	assert(instance_name != 0);

	DictEntry* entry =
		Dict_find(&(self->node_instance_dict), instance_name);

	if (!entry) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Node instance '%s' is not defined\n",
			instance_name->data
		);
		return 0;
	}

	return (Node*)entry->value;
}



int
animation_add_node_instance(
	Animation* self,
	const String* instance_name,
	const String* delegate_name
) {
	assert(self != 0);
	assert(instance_name != 0);
	assert(delegate_name != 0);

	// Check that there is no instance with that name already
	DictEntry* entry =
		Dict_find(&(self->node_instance_dict), instance_name);

	if (entry) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Node instance '%s' is already defined\n",
			instance_name->data
		);
		return 0;
	}

	// Create the node instance
	Node* node = node_create_by_name(instance_name, delegate_name);
	if (!node)
		return 0;

	// Update the instance dictionary
	Dict_insert(&(self->node_instance_dict), &(node->name))->value = node;
	
	// Job done
	return 1;
}


void
animation_handle_event(
	Animation* self,
	const Event* event
) {
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		node_handle_event(*node_ptr, event);
}


void
animation_render(
	const Animation* self,
	SDL_Surface* dst
) {
	renderer_render(self->renderer, node_get(self->sorted_nodes[0]), dst);
}


void
animation_update(
	Animation* self
) {
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		node_update(*node_ptr);
}