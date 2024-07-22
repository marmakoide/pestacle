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
	String name;
	real_t default_value;
} NodeParameterDefinition;


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

	size_t parameter_count;
	const NodeParameterDefinition* parameter_defs;

	NodeDelegateMethods methods;
} NodeDelegate;


extern const NodeDelegate*
node_delegate_list[];


typedef struct {
	real_t value;	
} NodeParameter;


struct s_Node {
	void* data;
	const NodeDelegate* delegate;
	Node** inputs;
	NodeParameter* parameters;
}; // struct s_Node


extern Node*
node_create_by_name(
	const String* name
);


extern int
node_set_input_slot_by_name(
	Node* self,
	const String* name,
	Node* other
);


NodeParameter*
node_get_parameter_by_name(
	Node* self,
	const String* name
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