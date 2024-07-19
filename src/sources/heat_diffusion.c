#include "sources/heat_diffusion.h"


struct HeatDiffusionData {
	int width;
	int height;

	real_t dt;
	real_t diffusion_coeff;
	real_t decay_coeff;

	struct Matrix U;
	struct Matrix U_tmp;
	struct Vector diff_kernel;
};



int
heat_diffusion_source_setup(
	struct Source* self,
	int width,
	int height
) {
	struct HeatDiffusionData* data =
		(struct HeatDiffusionData*)self->data;

	data->width = width;
	data->height = height;

	//  Animation params and variables
	data->dt = 1e-2f;
	data->diffusion_coeff = 1e-2f;
	data->decay_coeff = 1e-2f;

	Matrix_init(&(data->U), height, width);
	Matrix_fill(&(data->U), 0.f);

	Matrix_init(&(data->U_tmp), height, width);
	Matrix_fill(&(data->U_tmp), 0.f);

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

	Vector_scale(&(data->diff_kernel), 1. / Vector_sum(&data->diff_kernel));

	// Job done
	return 1;
}


void
heat_diffusion_source_destroy(
	struct Source* self
) {
	struct HeatDiffusionData* data =
		(struct HeatDiffusionData*)self->data;

	Matrix_destroy(&(data->U));
	Matrix_destroy(&(data->U_tmp));
	Vector_destroy(&(data->diff_kernel));
}


void
heat_diffusion_source_update(
	struct Source* self	
) {
	struct HeatDiffusionData* data =
		(struct HeatDiffusionData*)self->data;

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
		1.f - data->decay_coeff
	);
}


void
heat_diffusion_source_handle_event(
	struct Source* self,
	const union Event* event
) {
	struct HeatDiffusionData* data =
		(struct HeatDiffusionData*)self->data;

	switch(event->type) {
		case EventType_MouseMotion:
			Matrix_set_coeff(
				&(data->U),
				(size_t)floorf(event->mouse_motion.y),
				(size_t)floorf(event->mouse_motion.x),
				32.f);
			break;

		default:
			break;
	}
}


const struct Matrix*
heat_diffusion_source_get(
	const struct Source* self	
) {
	const struct HeatDiffusionData* data =
		(const struct HeatDiffusionData*)self->data;
	
	return &(data->U);
}


static const struct SourceDelegate
heat_diffusion_source_delegate = {
	"heat-diffusion",
	heat_diffusion_source_setup,
	heat_diffusion_source_destroy,
	heat_diffusion_source_update,
	heat_diffusion_source_handle_event,
	heat_diffusion_source_get
};



struct Source*
heat_diffusion_source_new() {
	// Allocation
	struct Source* ret = source_allocate();
	if (!ret)
		return ret;

	struct HeatDiffusionData* data =
		(struct HeatDiffusionData*)malloc(sizeof(struct HeatDiffusionData));

	// Delegate setup
	ret->data = data;
	ret->delegate = &heat_diffusion_source_delegate;

	// Job done
	return ret;
}