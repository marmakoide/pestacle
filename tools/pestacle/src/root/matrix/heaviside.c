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
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
};


#define THRESHOLD_PARAMETER 0

static const ParameterDefinition
node_parameters[] = {
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
	// Retrieve input data descriptor
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_INPUT]->out_descriptor);

	size_t width  = in_descriptor->matrix.width;
	size_t height = in_descriptor->matrix.height;

	// Setup input data descriptor
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_INPUT]), width, height
	);

	// Allocate data
	Matrix* data =
		(Matrix*)checked_malloc(sizeof(Matrix));

	if (!data)
		return false;

	// Setup data
	Matrix_init(data, height, width);
	Matrix_fill(data, (real_t)0);

	// Setup output descriptor
	DataDescriptor_set_as_matrix(&(self->out_descriptor), width, height);

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
