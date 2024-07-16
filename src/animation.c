#include "image.h"
#include "animation.h"

#include "renderers/gradient.h"
#include "renderers/linear_blend.h"


int
animation_init(
	struct Animation* self,
	int screen_width,
	int screen_height
) {
	self->screen_width = screen_width;
	self->screen_height = screen_height;
	self->cursor_x = 0;
	self->cursor_y = 0;

	//  Animation params and variables
	self->dt = 1e-2f;
	self->diffusion_coeff = 1e-2f;
	self->decay_coeff = 1e-2f;

	Matrix_init(&(self->U), self->screen_height, self->screen_width);
	Matrix_fill(&(self->U), 0.f);

	Matrix_init(&(self->U_tmp), self->screen_height, self->screen_width);
	Matrix_fill(&(self->U_tmp), 0.f);

	Matrix_init(&(self->U_laplacian), self->screen_height, self->screen_width);
	Matrix_fill(&(self->U_laplacian), 0.f);

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

	Vector_init(&(self->diff_kernel), 7);
	Vector_set_coeff(&(self->diff_kernel), 0,  1.f);
	Vector_set_coeff(&(self->diff_kernel), 1,  6.f);
	Vector_set_coeff(&(self->diff_kernel), 2,  15.f);
	Vector_set_coeff(&(self->diff_kernel), 3,  20.f);
	Vector_set_coeff(&(self->diff_kernel), 4,  15.f);
	Vector_set_coeff(&(self->diff_kernel), 5,  6.f);
	Vector_set_coeff(&(self->diff_kernel), 6,  1.f);
	Vector_scale(&(self->diff_kernel), 1. / Vector_sum(&(self->diff_kernel)));

	// Setup the renderer
	//self->renderer = gradient_renderer_new();
	
	self->renderer =
		linear_blend_renderer_new(
			"assets/paysage-vignoble_200x100.png",
			"assets/soif_200x100.png"
		);
		
	if (!renderer_setup(
		self->renderer,
		screen_width,
		screen_height
	))
		return 0;

	// Job done
	return 1;
}


void
animation_destroy(
	struct Animation* self
) {
	Matrix_destroy(&(self->U));
	Matrix_destroy(&(self->U_tmp));
	Matrix_destroy(&(self->U_laplacian));
	Vector_destroy(&(self->diff_kernel));

	renderer_destroy(self->renderer);

	#ifdef DEBUG
	self->renderer = 0;
	#endif
}


void
animation_handle_mouse_event(
	struct Animation* self,
	float x,
	float y
) {
	self->cursor_x = x;
	self->cursor_y = y;
}


void
animation_render(
	const struct Animation* self,
	SDL_Surface* dst
) {
	renderer_render(
		self->renderer,
		&(self->U),
		dst
	);
}


void
animation_update(
	struct Animation* self
) {
	// Set U to 1 at the cursor location
	Matrix_set_coeff(
		&(self->U),
		self->cursor_y,
		self->cursor_x,
		32.f);

	// Diffusion operator on U
	Matrix_rowwise_correlation(
		&(self->U),
		&(self->diff_kernel),
		&(self->U_tmp)
	);

	Matrix_colwise_correlation(
		&(self->U_tmp),
		&(self->diff_kernel),
		&(self->U)
	);
	
	// Apply decay
	Matrix_scale(
		&(self->U),
		1.f - self->decay_coeff
	);
}