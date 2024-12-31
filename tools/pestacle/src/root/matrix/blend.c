#include <SDL_log.h>
#include <pestacle/memory.h>

#include "root/matrix/blend.h"


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


#define SOURCE_A_INPUT 0
#define SOURCE_B_INPUT 1
#define MASK_INPUT     2


static const NodeInputDefinition
node_inputs[] = {
	{
		"source-a",
		true
	},
	{
		"source-b",
		true
	},
	{
		"mask",
		true
	},
	NODE_INPUT_DEFINITION_END
};


static const ParameterDefinition
node_parameters[] = {
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_blend_node_delegate = {
	"blend",
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
	// Retrieve input data descriptors
	const DataDescriptor* in_a_descriptor =
		&(self->inputs[SOURCE_A_INPUT]->out_descriptor);

	const DataDescriptor* in_b_descriptor =
		&(self->inputs[SOURCE_B_INPUT]->out_descriptor);

	const DataDescriptor* in_mask_descriptor =
		&(self->inputs[MASK_INPUT]->out_descriptor);

	size_t width  = in_a_descriptor->matrix.width;
	size_t height = in_a_descriptor->matrix.height;

	// Check input descriptors validity
	if
		((in_b_descriptor->matrix.width != width) ||
		 (in_b_descriptor->matrix.height != height) ||
		 (in_mask_descriptor->matrix.width != width) ||
		 (in_mask_descriptor->matrix.height != height)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"source-a, source-b and mask inputs should have the same dimensions"
			);
		return false;
	}

	// Allocate data
	Matrix* data =
		(Matrix*)checked_malloc(sizeof(Matrix));

	if (!data)
		return false;

	// Setup data
	Matrix_init(data, height, width);
	Matrix_fill(data, (real_t)0);

	// Setup output descriptor
	self->out_descriptor.type = DataType__matrix;
	self->out_descriptor.matrix.width = width;
	self->out_descriptor.matrix.height = height;

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
	// k * A + (1 - k) * B = k * A - k * B + B = k * (A - B) + B
	// Retrieve inputs and outputs
	const Matrix* src_a =
		Node_output(self->inputs[SOURCE_A_INPUT]).matrix;

	const Matrix* src_b =
		Node_output(self->inputs[SOURCE_B_INPUT]).matrix;

	const Matrix* mask =
		Node_output(self->inputs[MASK_INPUT]).matrix;

	Matrix* data = (Matrix*)self->data;

	Matrix_copy(data, src_a);
	Matrix_sub(data, src_b);
	Matrix_mul(data, mask);
	Matrix_add(data, src_b);
}


static NodeOutput
node_output(
	const Node* self
) {
	const Matrix* data = (const Matrix*)self->data;

	NodeOutput ret = { .matrix = data };
	return ret;
}
