#include <math.h>
#include <pestacle/memory.h>

#include "nodes/matrix_resize.h"


#ifndef M_PI
#   define M_PI 3.1415926535897932384626433832
#endif


// --- Interface --------------------------------------------------------------

static bool
matrix_resize_node_setup(
	Node* self
);


static void
matrix_resize_node_destroy(
	Node* self
);


static void
matrix_resize_node_update(
	Node* self
);


static NodeOutput
matrix_resize_node_output(
	const Node* self
);


#define SOURCE_INPUT 0

static const NodeInputDefinition
matrix_resize_inputs[] = {
	{
		NodeType__matrix,
		{ "source", 7 }
	},
	NODE_INPUT_DEFINITION_END
};


#define INPUT_WIDTH_PARAMETER   0
#define INPUT_HEIGHT_PARAMETER  1
#define OUTPUT_WIDTH_PARAMETER  2
#define OUTPUT_HEIGHT_PARAMETER 3

static const ParameterDefinition
matrix_resize_parameters[] = {
	{
		ParameterType__integer,
		{ "input-width", 12 },
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		{ "input-height", 13 },
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		{ "output-width", 13 },
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		{ "output-height", 14 },
		{ .int64_value = 32 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
matrix_resize_node_delegate = {
	{ "matrix-resize", 14 },
	NodeType__matrix,
	matrix_resize_inputs,
	matrix_resize_parameters,
	{
		matrix_resize_node_setup,
		matrix_resize_node_destroy,
		matrix_resize_node_update,
		matrix_resize_node_output
	},
};


// --- Implementation ---------------------------------------------------------

typedef struct {
	Matrix A;
	Matrix B;
	Matrix C;
	Matrix D;
	Matrix out;
	size_t box_filter_size_x;
	size_t box_filter_size_y;
} MatrixResizeData;


static void
matrix_resize_data_init(
	MatrixResizeData* self,
	size_t input_width,
	size_t input_height,
	size_t output_width,
	size_t output_height
) {
	Matrix_init(&(self->A), input_height, input_width);
	Matrix_init(&(self->B), input_height, input_width);
	Matrix_init(&(self->C), input_width, input_height);
	Matrix_init(&(self->D), input_width, input_height);

	Matrix_init(&(self->out), output_height, output_width);

	float x_factor = ((float)input_width) / ((float)output_width);
	float y_factor = ((float)input_height) / ((float)output_height);

	float kernel_x_sigma = (x_factor - 1) / 2;
	float kernel_y_sigma = (y_factor - 1) / 2;

	self->box_filter_size_x = (size_t)floorf(kernel_x_sigma * 3 * sqrtf(2 * M_PI) / 4 + ((real_t)0.5));
	self->box_filter_size_y = (size_t)floorf(kernel_y_sigma * 3 * sqrtf(2 * M_PI) / 4 + ((real_t)0.5));
}


static void
matrix_resize_data_destroy(
	MatrixResizeData* self
) {
	Matrix_destroy(&(self->A));
	Matrix_destroy(&(self->B));
	Matrix_destroy(&(self->C));
	Matrix_destroy(&(self->D));
	Matrix_destroy(&(self->out));
}


static bool
matrix_resize_node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t input_width   = (size_t)self->parameters[INPUT_WIDTH_PARAMETER].int64_value;
	size_t input_height  = (size_t)self->parameters[INPUT_HEIGHT_PARAMETER].int64_value;
	size_t output_width  = (size_t)self->parameters[OUTPUT_WIDTH_PARAMETER].int64_value;
	size_t output_height = (size_t)self->parameters[OUTPUT_HEIGHT_PARAMETER].int64_value;

	// Allocate
	MatrixResizeData* data =
		(MatrixResizeData*)checked_malloc(sizeof(MatrixResizeData));

	if (!data)
		return false;

	matrix_resize_data_init(
		data,
		input_width,
		input_height,
		output_width,
		output_height
	);

	// Setup node type metadata
	self->metadata.matrix.width = output_width;
	self->metadata.matrix.height = output_height;

	// Job done
	self->data = data;
	return true;
}


static void
matrix_resize_node_destroy(
	Node* self
) {
	MatrixResizeData* data = (MatrixResizeData*)self->data;
	if (data)
		matrix_resize_data_destroy(data);
}


static void
matrix_resize_node_update(
	Node* self
) {
	MatrixResizeData* data = (MatrixResizeData*)self->data;

	const Matrix* src =
		Node_output(self->inputs[SOURCE_INPUT]).matrix;

	// X filter
	Matrix_rowwise_box_filter(
		src,
		data->box_filter_size_x,
		&(data->A)
	);

	Matrix_rowwise_box_filter(
		&(data->A),
		data->box_filter_size_x,
		&(data->B)
	);

	Matrix_rowwise_box_filter(
		&(data->B),
		data->box_filter_size_x,
		&(data->A)
	);

	// Transposition
	Matrix_transpose(
		&(data->C),
		&(data->A)
	);

	// Y filter
	Matrix_rowwise_box_filter(
		&(data->C),
		data->box_filter_size_y,
		&(data->D)
	);

	Matrix_rowwise_box_filter(
		&(data->D),
		data->box_filter_size_y,
		&(data->C)
	);

	Matrix_rowwise_box_filter(
		&(data->C),
		data->box_filter_size_y,
		&(data->D)
	);

	// Transposition
	Matrix_transpose(
		&(data->D),
		&(data->A)
	);

	// Resample
	Matrix_resample_nearest(
		&(data->out),
		&(data->A)
	);
}


static NodeOutput
matrix_resize_node_output(
	const Node* self
) {
	MatrixResizeData* data = (MatrixResizeData*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
