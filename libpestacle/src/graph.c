#include <assert.h>
#include <pestacle/stack.h>
#include <pestacle/graph.h>
#include <pestacle/memory.h>
#include <pestacle/tree_map.h>


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
Scope_gather_all_nodes(
	Scope* self,
	Stack* out
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
				printf("gathered %s\n", member->node->name);
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
	Scope* scope
) {
	// Allocate
	TreeMap map;
	TreeMap_init(&map);

	Stack stack;
	Stack_init(&stack);

	Stack ret;
	Stack_init(&ret);

	// All nodes are marked as unvisited
	Scope_gather_all_nodes(scope, &stack);
	for(size_t i = 0; i < Stack_length(&stack); ++i)
		TreeMap_insert(&map, stack.data[i]);

	// While we have unvisited nodes
	while(!TreeMap_empty(&map)) {
		// Traverse the unvisited nodes
		Stack_clear(&stack);
		for(int i = 0; i < 2; ++i)
			if (map.root.child[i] != &(map.nil))
				Stack_push(&stack, map.root.child[i]);

		while(!Stack_empty(&stack)) {
			TreeMapNode* it = (TreeMapNode*)Stack_pop(&stack);
			Node* node = (Node*)it->key;

			// Node have all his inputs visited
			bool all_inputs_unmarked = true;
			Node** input_ptr = node->inputs;
			const NodeInputDefinition* input_def = node->delegate->input_defs;
			for( ; !NodeInputDefinition_is_last(input_def); ++input_ptr, ++input_def) {
				if (*input_ptr != 0) {
					TreeMapNode* it = TreeMap_find(&map, *input_ptr);
					if (it)
						all_inputs_unmarked = false;
				}
			}

			// Node have no inputs
			if (all_inputs_unmarked) {
				Stack_push(&ret, node);
				printf("push node %s\n", node->name);
				TreeMap_erase(&map, it);
				break;
			}
			
			for(int i = 0; i < 2; ++i)
				if (it->child[i] != &(map.nil))
					Stack_push(&stack, it->child[i]);
		}
	}

	//
	self->sorted_node_count = Stack_length(&ret);
	self->sorted_nodes = (Node**)checked_malloc(Stack_length(&ret) * sizeof(Node*));
	for(size_t i = 0; i < Stack_length(&ret); ++i)
		self->sorted_nodes[i] = ret.data[i];

	// Job done
	TreeMap_destroy(&map);
	Stack_destroy(&stack);
	Stack_destroy(&ret);
	return true;
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

	// Sort the nodes
	if (!Graph_topological_sort(self, scope))
		goto failure;

	// Check validity
	if (!Graph_check_graph_is_complete(self))
		goto failure;

	// Job done
	return true;

failure:
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

	// Setup the nodes in topological order
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr) {
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

	// Update the nodes in topological order
	Node** node_ptr = self->sorted_nodes;
	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr)
		Node_update(*node_ptr);
}


void
Graph_update_with_profile(
	Graph* self,
	GraphProfile* profile
) {
	assert(self);
	assert(profile);

	// Update the nodes in topological order
	Node** node_ptr = self->sorted_nodes;
	NodeProfile* profile_ptr = profile->node_profiles;

	Uint64 start_time = SDL_GetPerformanceCounter();

	for(size_t i = self->sorted_node_count; i != 0; --i, ++node_ptr, ++profile_ptr) {
		// Update the node
		Uint64 node_start_time = SDL_GetPerformanceCounter();
		Node_update(*node_ptr);
		Uint64 node_end_time = SDL_GetPerformanceCounter();

		// Track the running time for that node
		NodeProfile_update(
			profile_ptr,
			((real_t)(node_end_time - node_start_time)) / SDL_GetPerformanceFrequency()
		);
	}

	Uint64 end_time = SDL_GetPerformanceCounter();
	AverageResult_accumulate(
		&(profile->time),
		((real_t)(end_time - start_time)) / SDL_GetPerformanceFrequency()
	);
}
