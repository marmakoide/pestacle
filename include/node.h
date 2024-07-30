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


// --- Node I/O definitions ---------------------------------------------------

enum NodeType {
	NodeType__invalid = 0, // Used as a debugging help
	NodeType__matrix,
	NodeType__rgb_surface,
	NodeType__last         // Used to mark the end of an array of NodeInputType
}; // enum NodeType


typedef struct {
	enum NodeType type;
	String name;
} NodeInputDefinition;


typedef union {
	const Matrix* matrix;
	SDL_Surface* rgb_surface;
} NodeOutput;


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

	NodeOutput (*get)(      // get method
		const Node*
	);
} NodeDelegateMethods;


typedef struct {
	String name;
	enum NodeType type;
	const NodeInputDefinition* input_defs;
	const NodeParameterDefinition* parameter_defs;
	NodeDelegateMethods methods;
} NodeDelegate;


struct s_Node {
	void* data;
	const NodeDelegate* delegate;
	String name;
	Node** inputs;
	NodeParameterValue* parameters;
}; // struct s_Node


/*
 * Returns the node delegate with the specified name
 *   name : name of the node delegate to retrieve
 *
 * Return 0 if the operation is not valid
 */

extern const NodeDelegate*
get_node_delegate_by_name(const String* name);


/*
 * Creates a new node instance
 *   name : name of the instance, will make a copy of the string instance
 *   delegate : delegate for this node
 */

extern Node*
Node_new(
	const String* name,
	const NodeDelegate* delegate
);


/*
 * Connect a node input to the output of an other node ie. other -> self.name
 *   self : node to which an input will be connected
 *   name : name of the input to be connected
 *   other : node to which the output will be connected
 *
 * Return false if the operation is invalid : no input with the specified name 
 * and compatible types
 */
 
extern bool
Node_set_input_by_name(
	Node* self,
	const String* name,
	Node* other
);


/*
 * Returns a node's parameter definiton and value
 *   self : the node
 *   name : name of the parameter
 *   node_def_ptr : node parameter definition parameter
 *   node_value_ptr : node parameter
 *
 * Returns false if the operation is invalid : no parameter with the specified
 * name. Outputs to values pointed by node_def_ptr and node_value_ptr, if not
 * 0.
 */

bool
Node_get_parameter_by_name(
	Node* self,
	const String* name,
	const NodeParameterDefinition** node_def_ptr,
	NodeParameterValue** node_value_ptr
);


/*
 * Returns true if all the inputs of a node are connected
 */

extern bool
Node_is_complete(
	const Node* self
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


extern void
Node_update(
	Node* self
);


extern void
Node_handle_event(
	Node* self,
	const Event* event
);


extern NodeOutput
Node_get(
	Node* self
);


#endif /* PESTACLE_NODE_H */