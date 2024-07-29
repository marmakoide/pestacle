#ifndef PESTACLE_NODE_H
#define PESTACLE_NODE_H

#include <SDL.h>
#include "event.h"
#include "matrix.h"
#include "strings.h"


// --- Node parameter definitons ----------------------------------------------

enum NodeParameterType {
	NodeParameterType__invalid = 0, // Used as a debugging help
	NodeParameterType__integer,
	NodeParameterType__real,
	NodeParameterType__string,
	NodeParameterType__last         // Used to mark the end of an array of NodeParameterType
}; // enum NodeParameterType


typedef union {
	int64_t int64_value;
	real_t real_value;
	String string_value;
} NodeParameterValue;


typedef struct {
	enum NodeParameterType type;
	String name;
	NodeParameterValue default_value;
} NodeParameterDefinition;


// --- Node input definitions -------------------------------------------------

enum NodeInputType {
	NodeInputType__invalid = 0, // Used as a debugging help
	NodeInputType__matrix,
	NodeInputType__rgb_surface,
	NodeInputType__last         // Used to mark the end of an array of NodeInputType
}; // enum NodeParameterType


typedef struct {
	enum NodeInputType type;
	String name;
} NodeInputDefinition;


// --- Node definitions -------------------------------------------------------

struct s_Node;
typedef struct s_Node Node;


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
	const NodeInputDefinition* input_defs;
	const NodeParameterDefinition* parameter_defs;
	NodeDelegateMethods methods;
} NodeDelegate;


extern const NodeDelegate*
get_node_delegate_by_name(const String* name);


struct s_Node {
	void* data;
	const NodeDelegate* delegate;
	String name;
	Node** inputs;
	NodeParameterValue* parameters;
}; // struct s_Node


extern Node*
Node_create_by_name(
	const String* name,
	const String* delegate_name
);


extern bool
Node_set_input_by_name(
	Node* self,
	const String* name,
	Node* other
);


bool
Node_get_parameter_by_name(
	Node* self,
	const String* name,
	const NodeParameterDefinition** node_def_ptr,
	NodeParameterValue** node_value_ptr
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