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
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
};


#define DECAY_PARAMETER  0

static const ParameterDefinition
node_parameters[] = {
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
	// Retrieve input data descriptor
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_INPUT]->out_descriptor);

	size_t width  = in_descriptor->matrix.width;
	size_t height = in_descriptor->matrix.height;

	// Allocate data
	HeatDiffusionData* data =
		(HeatDiffusionData*)checked_malloc(sizeof(HeatDiffusionData));

	if (!data)
		return false;

	// Setup output descriptor
	self->out_descriptor.type = DataType__matrix;
	self->out_descriptor.matrix.width = width;
	self->out_descriptor.matrix.height = height;

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

	Vector_scale(&(data->diff_kernel), 1.f / Vector_reduction_sum(&data->diff_kernel));

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	if (data) {
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
