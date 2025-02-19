#include <pestacle/memory.h>

#include "root/matrix/crop.h"


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
#define X_PARAMETER      2
#define Y_PARAMETER      3

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
		ParameterType__integer,
		"x",
		{ .int64_value = 0 }
	},
	{
		ParameterType__integer,
		"y",
		{ .int64_value = 0 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_crop_node_delegate = {
	"crop",
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

	// Fill the output with zeros
	Matrix_fill(dst, (real_t)0);

	// Compute clipping
	int64_t x0 = (real_t)self->parameters[X_PARAMETER].int64_value;
	int64_t x1 = x0;
	x1 += dst->col_count;

	if ((x1 <= 0) || (x0 >= (int64_t)src->col_count))
		return;

	int64_t y0 = (real_t)self->parameters[Y_PARAMETER].int64_value;
	int64_t y1 = y0;
	y1 += dst->row_count;

	if ((y1 <= 0) || (y0 >= (int64_t)src->row_count))
		return;

	if (x0 < 0)
		x0 = 0;

	if (x1 >= (int64_t)src->col_count)
		x1 = src->col_count - 1;

	if (y0 < 0)
		y0 = 0;

	if (y1 >= (int64_t)src->row_count)
		y1 = src->row_count - 1;

	// Copy input data
	size_t width = (size_t)(x1 - x0);
	size_t height = (size_t)(y1 - y0);

	const real_t* src_data = src->data;
	src_data += y0 * src->col_count;
	src_data += x0;

	real_t* dst_data = dst->data;

	for(size_t i = height; i != 0; --i, src_data += src->col_count, dst_data += dst->col_count)
		array_ops_copy(dst_data, src_data, width);
}


static NodeOutput
node_output(
	const Node* self
) {
	Matrix* data = (Matrix*)self->data;

	NodeOutput ret = { .matrix = data };
	return ret;
}
