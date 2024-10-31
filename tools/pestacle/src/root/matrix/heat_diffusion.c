#include <pestacle/memory.h>

#include "root/matrix/heat_diffusion.h"


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
		"source"
	},
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1
#define DECAY_PARAMETER  2

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
		"decay",
		{ .real_value = 1e-2f }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_heat_diffusion_node_delegate = {
	"heat-diffusion",
	NodeType__matrix,
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
	Matrix U;
	Matrix U_tmp;
	Vector diff_kernel;
} HeatDiffusionData;


static bool
node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate data
	HeatDiffusionData* data =
		(HeatDiffusionData*)checked_malloc(sizeof(HeatDiffusionData));

	if (!data)
		return false;

	// Setup node type metadata
	self->metadata.matrix.width = width;
	self->metadata.matrix.height = height;

	// Setup data
	Matrix_init(&(data->U), height, width);
	Matrix_fill(&(data->U), (real_t)0);

	Matrix_init(&(data->U_tmp), height, width);
	Matrix_fill(&(data->U_tmp), (real_t)0);

	/*
	Vector_init(&(self->diff_kernel), 3);
	Vector_set_coeff(&(self->diff_kernel), 0,  0.25f);
	Vector_set_coeff(&(self->diff_kernel), 1,  0.50f);
	Vector_set_coeff(&(self->diff_kernel), 2,  0.25f);
	*/

	/*
	Vector_init(&(self->diff_kernel), 5);
	Vector_set_coeff(&(self->diff_kernel), 0,  1.f / 16.f);
	Vector_set_coeff(&(self->diff_kernel), 1,  4.f / 16.f);
	Vector_set_coeff(&(self->diff_kernel), 2,  6.f / 16.f);
	Vector_set_coeff(&(self->diff_kernel), 3,  4.f / 16.f);
	Vector_set_coeff(&(self->diff_kernel), 4,  1.f / 16.f);
	*/

	Vector_init(&(data->diff_kernel), 7);
	Vector_set_coeff(&(data->diff_kernel), 0,  1.f);
	Vector_set_coeff(&(data->diff_kernel), 1,  6.f);
	Vector_set_coeff(&(data->diff_kernel), 2,  15.f);
	Vector_set_coeff(&(data->diff_kernel), 3,  20.f);
	Vector_set_coeff(&(data->diff_kernel), 4,  15.f);
	Vector_set_coeff(&(data->diff_kernel), 5,  6.f);
	Vector_set_coeff(&(data->diff_kernel), 6,  1.f);

	Vector_scale(&(data->diff_kernel), 1.f / Vector_sum(&data->diff_kernel));

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	if (data != 0) {
		Matrix_destroy(&(data->U));
		Matrix_destroy(&(data->U_tmp));
		Vector_destroy(&(data->diff_kernel));

		free(data);
	}
}


static void
node_update(
	Node* self
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	real_t decay = self->parameters[DECAY_PARAMETER].real_value;

	// Update U with input
	Matrix_max(
		&(data->U),
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);

	// Diffusion operator on U
	Matrix_rowwise_convolution__zero(
		&(data->U_tmp),
		&(data->U),
		&(data->diff_kernel)
	);

	Matrix_colwise_convolution__zero(
		&(data->U),
		&(data->U_tmp),
		&(data->diff_kernel)
	);
	
	// Apply decay
	Matrix_scale(
		&(data->U),
		((real_t)1) - decay
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	const HeatDiffusionData* data = (const HeatDiffusionData*)self->data;

	NodeOutput ret = { .matrix = &(data->U) };
	return ret;
}
