#include "sources/heat_diffusion.h"


typedef struct {
	real_t dt;
	real_t diffusion_coeff;
	real_t decay_coeff;

	Matrix U;
	Matrix U_tmp;
	Vector diff_kernel;
} HeatDiffusionData;



int
heat_diffusion_source_setup(
	Source* self,
	int width,
	int height
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	//  Animation params and variables
	data->dt = 1e-2f;
	data->diffusion_coeff = 1e-2f;
	data->decay_coeff = 1e-2f;

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
	return 1;
}


void
heat_diffusion_source_destroy(
	Source* self
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	Matrix_destroy(&(data->U));
	Matrix_destroy(&(data->U_tmp));
	Vector_destroy(&(data->diff_kernel));
}


void
heat_diffusion_source_update(
	Source* self	
) {
	HeatDiffusionData* data = (HeatDiffusionData*)self->data;

	// Update U with input
	Matrix_max(
		&(data->U),
		source_get(self->inputs[0])
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
		((real_t)1) - data->decay_coeff
	);
}


const Matrix*
heat_diffusion_source_get(
	const Source* self	
) {
	const HeatDiffusionData* data = (const HeatDiffusionData*)self->data;
	
	return &(data->U);
}

static const SourceInputSlotDefinition
heat_diffusion_inputs[] = {
	{
		{ "input", 6 }
	}
};


static const SourceDelegate
heat_diffusion_source_delegate = {
	{ "heat-diffusion", 15 },

	1, heat_diffusion_inputs,

	{
		heat_diffusion_source_setup,
		heat_diffusion_source_destroy,
		heat_diffusion_source_update,
		0,
		heat_diffusion_source_get
	},
};



Source*
heat_diffusion_source_new() {
	// Allocation
	Source* ret = source_allocate();
	if (!ret)
		return ret;

	HeatDiffusionData* data =
		(HeatDiffusionData*)malloc(sizeof(HeatDiffusionData));

	// Initialisation
	source_init(ret, &heat_diffusion_source_delegate, data);

	// Job done
	return ret;
}