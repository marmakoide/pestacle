#include "nodes/heat_diffusion.h"
#include "memory.h"


// --- Interface --------------------------------------------------------------

static int
heat_diffusion_node_setup(
	Node* self,
	int width,
	int height
);


static void
heat_diffusion_node_destroy(
	Node* self
);


static void
heat_diffusion_node_update(
	Node* self	
);


static const Matrix*
heat_diffusion_node_get(
	const Node* self
);


static const NodeInputSlotDefinition
heat_diffusion_inputs[] = {
	{
		{ "input", 6 }
	}
};


#define DECAY_PARAMETER 0

static const NodeParameterDefinition
heat_diffusion_parameters[] = {
	{
		{ "decay", 6 },
		1e-2f
	}
};


const NodeDelegate
heat_diffusion_node_delegate = {
	{ "heat-diffusion", 15 },

	1, heat_diffusion_inputs,

	1, heat_diffusion_parameters,

	{
		heat_diffusion_node_setup,
		heat_diffusion_node_destroy,
		heat_diffusion_node_update,
		0,
		heat_diffusion_node_get
	},
};


// --- Implementation ---------------------------------------------------------

typedef struct {
	Matrix U;
	Matrix U_tmp;
	Vector diff_kernel;
} HeatDiffusionData;



static int
heat_diffusion_node_setup(
	Node* self,
	int width,
	int height
) {
	// Allocate
	HeatDiffusionData* data =
		(HeatDiffusionData*)checked_malloc(sizeof(HeatDiffusionData));

	if (!data)
		return 0;

	// Setup
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
	return 1;
}


static void
heat_diffusion_node_destroy(
	Node* self
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	Matrix_destroy(&(data->U));
	Matrix_destroy(&(data->U_tmp));
	Vector_destroy(&(data->diff_kernel));

	free(data);
}


static void
heat_diffusion_node_update(
	Node* self
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	real_t decay = self->parameters[DECAY_PARAMETER].value;

	// Update U with input
	Matrix_max(
		&(data->U),
		node_get(self->inputs[0])
	);

	// Diffusion operator on U
	Matrix_rowwise_correlation(
		&(data->U),
		&(data->diff_kernel),
		&(data->U_tmp)
	);

	Matrix_colwise_correlation(
		&(data->U_tmp),
		&(data->diff_kernel),
		&(data->U)
	);
	
	// Apply decay
	Matrix_scale(
		&(data->U),
		((real_t)1) - decay
	);
}


static const Matrix*
heat_diffusion_node_get(
	const Node* self
) {
	const HeatDiffusionData* data = (const HeatDiffusionData*)self->data;
	
	return &(data->U);
}
