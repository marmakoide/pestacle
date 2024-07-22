#ifndef PESTACLE_NODE_H
#define PESTACLE_NODE_H

#include <SDL.h>
#include "event.h"
#include "matrix.h"
#include "strings.h"


struct s_Node;
typedef struct s_Node Node;


typedef struct {
	String name;
} NodeInputSlotDefinition;


typedef struct {
	int (*setup)(              // setup method (optional, can be 0)
		Node*,
		int width,
		int height
	);

	void (*destroy)(           // destroy method (optional, can be 0)
		Node*
	);

	void (*update)(            // update method (optional, can be 0)
		Node*
	);

	void (*handle_event)(      // handle_event (optional, can be 0)
		Node*,
		const Event* event	
	);

	const Matrix* (*get)(      // get method
		const Node*
	);
} NodeDelegateMethods;


typedef struct {
	String name;
	size_t input_count;
	const NodeInputSlotDefinition* input_defs;
	NodeDelegateMethods methods;
} NodeDelegate;


struct s_Node {
	void* data;
	const NodeDelegate* delegate;
	Node** inputs;
}; // struct s_Node


extern Node*
node_allocate();


extern void
node_init(
	Node* self,
	const NodeDelegate* delegate,
	void* data
);


extern int
node_set_input_slot(
	Node* self,
	const String* name,
	Node* other
);


extern int
node_setup(
	Node* self,
	int width,
	int height
);


extern void
node_destroy(
	Node* self
);


extern void
node_update(
	Node* self
);


extern void
node_handle_event(
	Node* self,
	const Event* event
);


extern const Matrix*
node_get(
	Node* self
);


#endif /* PESTACLE_NODE_H */