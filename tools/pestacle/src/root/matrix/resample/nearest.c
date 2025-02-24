#include <pestacle/memory.h>

#include "root/matrix/resample/nearest.h"


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


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1

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
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_resample_nearest_node_delegate = {
	"nearest",
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
	// Retrieve input data descriptors
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_INPUT]->out_descriptor);

	size_t input_width  = in_descriptor->matrix.width;
	size_t input_height = in_descriptor->matrix.height;

	// Setup input data descriptor
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_INPUT]), input_width, input_height
	);

	// Retrieve the parameters
	size_t output_width  = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t output_height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate
	Matrix* data = (Matrix*)checked_malloc(sizeof(Matrix));
	Matrix_init(data, output_height, output_width);

	// Setup output descriptor
	DataDescriptor_set_as_matrix(
		&(self->out_descriptor),
		output_width, 
		output_height
	);

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	Matrix* data = (Matrix*)self->data;
	if (data)
		Matrix_destroy(data);
}


static void
node_update(
	Node* self
) {
	const Matrix* src = Node_output(self->inputs[SOURCE_INPUT]).matrix;
	Matrix* dst = (Matrix*)self->data;

	Matrix_resample_nearest(dst, src);
}


static NodeOutput
node_output(
	const Node* self
) {
	Matrix* data = (Matrix*)self->data;

	NodeOutput ret = { .matrix = data };
	return ret;
}
