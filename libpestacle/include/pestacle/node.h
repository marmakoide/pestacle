#ifndef PESTACLE_NODE_H
#define PESTACLE_NODE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <SDL.h>
#include <pestacle/matrix.h>
#include <pestacle/parameter.h>


// --- Node I/O definitions ---------------------------------------------------

enum NodeType {
	NodeType__invalid = 0, // Used as a debugging help
	NodeType__void,
	NodeType__matrix,
	NodeType__rgb_surface,
	NodeType__last         // Used to mark the end of an array of NodeInputType
}; // enum NodeType


typedef struct {
	enum NodeType type;
	String name;
} NodeInputDefinition;


#define NODE_INPUT_DEFINITION_END \
{ \
	NodeType__last, \
	{ 0, 0 }, \
}


typedef struct {
	size_t width;
	size_t height;
} NodeTypeMatrixMetadata;


typedef struct {
	size_t width;
	size_t height;
} NodeTypeRGBSurfaceMetadata;


typedef union {
	NodeTypeMatrixMetadata matrix;
	NodeTypeRGBSurfaceMetadata rgb_surface;
} NodeTypeMetadata;


typedef union {
	const Matrix* matrix;
	SDL_Surface* rgb_surface;
} NodeOutput;


// --- Node definitions -------------------------------------------------------

struct s_Scope;

struct s_Node;
typedef struct s_Node Node;


typedef struct {
	bool (*setup)(        // setup method (optional, can be 0)
		Node*
	);

	void (*destroy)(      // destroy method (optional, can be 0)
		Node*
	);

	void (*update)(       // update method (optional, can be 0)
		Node*
	);

	NodeOutput (*output)( // output method (optional, can be 0)
		const Node*
	);
} NodeDelegateMethods;


typedef struct {
	String name;
	enum NodeType type;
	const NodeInputDefinition* input_defs;
	const ParameterDefinition* parameter_defs;
	NodeDelegateMethods methods;
} NodeDelegate;


struct s_Node {
	void* data;
	String name;

	const NodeDelegate* delegate;
	struct s_Scope* delegate_scope; // Scope owning the delegate

	NodeTypeMetadata metadata;
	Node** inputs;
	ParameterValue* parameters;
}; // struct s_Node


/*
 * Creates a new node instance
 *   name : name of the instance, will make a copy of the string instance
 *   delegate : delegate for this node
  *  delegate_scope : scope owning the delegate
 */

extern Node*
Node_new(
	const String* name,
	const NodeDelegate* delegate,
	struct s_Scope* delegate_scope
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
 * Returns a node's parameter definition and value
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
	const ParameterDefinition** param_def_ptr,
	ParameterValue** param_value_ptr
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
	Node* self
);


extern void
Node_destroy(
	Node* self
);


extern void
Node_update(
	Node* self
);


extern NodeOutput
Node_output(
	Node* self
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_NODE_H */
