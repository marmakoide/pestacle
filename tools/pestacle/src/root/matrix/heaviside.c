#include <pestacle/memory.h>

#include "root/matrix/heaviside.h"


// --- Interface --------------------------------------------------------------

static bool
node_setup(
	Node* self
);


static void
node_destroy(
	Node* self
);


static void
node_update(
	Node* self
);


static NodeOutput
node_output(
	const Node* self
);


#define SOURCE_INPUT 0

static const NodeInputDefinition
node_inputs[] = {
	{
		NodeType__matrix,
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER     0
#define HEIGHT_PARAMETER    1
#define THRESHOLD_PARAMETER 2

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__integer,
		"width",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"height",
		{ .int64_value = 32 }
	},
	{
		ParameterType__real,
		"threshold",
		{ .real_value = (real_t)0 }
	},	
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_heaviside_node_delegate = {
	"heaviside",
	true,
	node_inputs,
	node_parameters,
	{
		node_setup,
		node_destroy,
		node_update,
		node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate data
	Matrix* data =
		(Matrix*)checked_malloc(sizeof(Matrix));

	if (!data)
		return false;

	// Setup data
	Matrix_init(data, height, width);
	Matrix_fill(data, (real_t)0);

	// Setup node type metadata
	self->type = NodeType__matrix;
	self->type_metadata.matrix.width = width;
	self->type_metadata.matrix.height = height;

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	Matrix* data = (Matrix*)self->data;

	if (data) {
		Matrix_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	real_t threshold = (real_t)self->parameters[THRESHOLD_PARAMETER].real_value;

	Matrix* data = (Matrix*)self->data;

	Matrix_copy(
		data,
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);
	Matrix_heaviside(
		data,
		threshold
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	const Matrix* data = (const Matrix*)self->data;

	NodeOutput ret = { .matrix = data };
	return ret;
}
