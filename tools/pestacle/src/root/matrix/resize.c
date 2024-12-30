#include <tgmath.h>
#include <pestacle/memory.h>

#include "root/matrix/resize.h"

/*
  See "Fast Almost-Gaussian Filtering" by Peter Kovesi for the Gaussian 
  filtering approximation
 */


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


#define INPUT_WIDTH_PARAMETER   0
#define INPUT_HEIGHT_PARAMETER  1
#define OUTPUT_WIDTH_PARAMETER  2
#define OUTPUT_HEIGHT_PARAMETER 3

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__integer,
		"input-width",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"input-height",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"output-width",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"output-height",
		{ .int64_value = 32 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_resize_node_delegate = {
	"resize",
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

typedef struct {
	Matrix A;
	Matrix B;
	Matrix C;
	Matrix D;
	Matrix out;
	size_t lo_filter_size_x;
	size_t hi_filter_size_x;
	size_t lo_filter_size_y;
	size_t hi_filter_size_y;
} MatrixResizeData;


#define PASS_COUNT 3

static real_t
get_ideal_filter_size(
	real_t sigma,
	int pass_count
) {
	return sqrt(((real_t)1) + (((real_t)12) * (sigma * sigma)) / pass_count);
}


static size_t
get_lo_filter_size(
	real_t ideal_filter_size
) {
	size_t ret = floor(ideal_filter_size);
	if (ret % 2 == 0)
		ret -= 1;
	return ret;
}


static size_t
get_hi_filter_size(
	real_t ideal_filter_size
) {
	size_t ret = ceil(ideal_filter_size);
	if (ret % 2 == 0)
		ret += 1;
	return ret;
}


static void
matrix_resize_data_init(
	MatrixResizeData* self,
	size_t input_width,
	size_t input_height,
	size_t output_width,
	size_t output_height
) {
	Matrix_init(&(self->A), input_height, input_width);
	Matrix_fill(&(self->A), (real_t)0);

	Matrix_init(&(self->B), input_height, input_width);
	Matrix_fill(&(self->B), (real_t)0);

	Matrix_init(&(self->C), input_width, input_height);
	Matrix_fill(&(self->C), (real_t)0);

	Matrix_init(&(self->D), input_width, input_height);
	Matrix_fill(&(self->D), (real_t)0);

	Matrix_init(&(self->out), output_height, output_width);
	Matrix_fill(&(self->out), (real_t)0);

	real_t x_factor = ((float)input_width) / ((float)output_width);
	real_t y_factor = ((float)input_height) / ((float)output_height);

	real_t kernel_x_sigma = (x_factor - 1) / 2;
	real_t kernel_y_sigma = (y_factor - 1) / 2;

	real_t ideal_filter_size_x = get_ideal_filter_size(kernel_x_sigma, PASS_COUNT);
	real_t ideal_filter_size_y = get_ideal_filter_size(kernel_y_sigma, PASS_COUNT);

	self->lo_filter_size_x = get_lo_filter_size(ideal_filter_size_x);
	self->lo_filter_size_y = get_lo_filter_size(ideal_filter_size_y);
	self->hi_filter_size_x = get_hi_filter_size(ideal_filter_size_x);
	self->hi_filter_size_y = get_hi_filter_size(ideal_filter_size_y);
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
node_setup(
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
	self->type = NodeType__matrix;
	self->type_metadata.matrix.width = output_width;
	self->type_metadata.matrix.height = output_height;

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	MatrixResizeData* data = (MatrixResizeData*)self->data;
	if (data)
		matrix_resize_data_destroy(data);
}


static void
node_update(
	Node* self
) {
	MatrixResizeData* data = (MatrixResizeData*)self->data;

	const Matrix* src =
		Node_output(self->inputs[SOURCE_INPUT]).matrix;

	// A = box-filter(src)
	Matrix_rowwise_box_filter(
		&(data->A),
		src,
		data->lo_filter_size_x
	);

	// B = box-filter(A)
	Matrix_rowwise_box_filter(
		&(data->B),
		&(data->A),
		data->hi_filter_size_x
	);

	// A = box-filter(B)
	Matrix_rowwise_box_filter(
		&(data->A),
		&(data->B),
		data->hi_filter_size_x
	);

	// C = transpose(A)
	Matrix_transpose(
		&(data->C),
		&(data->A)
	);

	// D = box-filter(C)
	Matrix_rowwise_box_filter(
		&(data->D),
		&(data->C),
		data->lo_filter_size_y
	);

	// C = box-filter(D)
	Matrix_rowwise_box_filter(
		&(data->C),
		&(data->D),
		data->hi_filter_size_y
	);

	// D = box-filterC)
	Matrix_rowwise_box_filter(
		&(data->D),
		&(data->C),
		data->hi_filter_size_y
	);

	// A = transpose(D)
	Matrix_transpose(
		&(data->A),
		&(data->D)
	);

	// out = resample(A)
	Matrix_resample_nearest(
		&(data->out),
		&(data->A)
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	MatrixResizeData* data = (MatrixResizeData*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
