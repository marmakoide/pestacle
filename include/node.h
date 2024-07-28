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
	bool (*setup)(             // setup method (optional, can be 0)
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
	String name;
	Node** inputs;
	NodeParameter* parameters;
}; // struct s_Node


extern Node*
Node_create_by_name(
	const String* name,
	const String* delegate_name
);


extern bool
Node_set_input_slot_by_name(
	Node* self,
	const String* name,
	Node* other
);


NodeParameter*
Node_get_parameter_by_name(
	Node* self,
	const String* name
);


extern bool
Node_setup(
	Node* self,
	int width,
	int height
);


extern void
Node_destroy(
	Node* self
);


extern bool
Node_is_complete(
	const Node* self
);


extern void
Node_update(
	Node* self
);


extern void
Node_handle_event(
	Node* self,
	const Event* event
);


extern const Matrix*
Node_get(
	Node* self
);


#endif /* PESTACLE_NODE_H */