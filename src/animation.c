#include "image.h"
#include "animation.h"


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

	// Load background picture
	self->picture_a = load_png("assets/paysage-vignoble_200x100.png");
	if (!self->picture_a) {
		self->picture_a = 0;
		return 0;
	}

	self->picture_b = load_png("assets/soif_200x100.png");
	if (!self->picture_b) {
		self->picture_b = 0;
		return 0;
	}

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

	if (self->picture_a)
		SDL_FreeSurface(self->picture_a);
	self->picture_a = 0;

	if (self->picture_b)
		SDL_FreeSurface(self->picture_b);
	self->picture_b = 0;
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
	SDL_Surface* surface
) {
	const real_t* coeff = self->U.data;
	uint8_t* pixel_row = (uint8_t*)surface->pixels;
	for(int i = self->screen_height; i != 0; --i, pixel_row += surface->pitch) {
		uint8_t* pixel = pixel_row;
		for(int j = self->screen_width; j != 0; --j, pixel += 3, ++coeff) {
			uint8_t level = (uint8_t)fmaxf(fminf(255.f * (*coeff), 255.f), 0.f);

			for(int k = 0; k < 3; ++k)
				pixel[k] = 0xff - level;
		}
	}

	/*
	const real_t* coeff = self->U.data;
	uint8_t* dst_pixel_row = (uint8_t*)surface->pixels;
	const uint8_t* src_a_pixel_row = (const uint8_t*)self->picture_a->pixels;
	const uint8_t* src_b_pixel_row = (const uint8_t*)self->picture_b->pixels;
	
	for(int i = self->screen_height; i != 0; --i, dst_pixel_row += surface->pitch, src_a_pixel_row += self->picture_a->pitch, src_b_pixel_row += self->picture_b->pitch) {
		uint8_t* dst_pixel = dst_pixel_row;
		const uint8_t* src_a_pixel = src_a_pixel_row;
		const uint8_t* src_b_pixel = src_b_pixel_row;

		for(int j = self->screen_width; j != 0; --j, dst_pixel += 3, src_a_pixel += 4, src_b_pixel += 4, ++coeff) {
			float level = fmaxf(fminf(*coeff, 1.f), 0.f);
			
			for(int k = 0; k < 3; ++k)
				dst_pixel[k] = (uint8_t)(src_b_pixel[k] * level + src_a_pixel[k] * (1.f - level));
		}
	}
	*/
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