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
		&(self->inputs[SOURCE_A_INPUT]->out_descriptor);

	// Setup input data descriptor
	size_t width  = in_descriptor->matrix.width;
	size_t height = in_descriptor->matrix.height;

	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_A_INPUT]), width, height
	);
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_B_INPUT]), width, height
	);
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[MASK_INPUT]), width, height
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
