#ifndef PESTACLE_ANIMATION_H
#define PESTACLE_ANIMATION_H

#include <SDL.h>
#include <vector.h>
#include <matrix.h>
#include "renderer.h"


struct Animation {
	int screen_width;
	int screen_height;
	size_t cursor_x;
	size_t cursor_y;

	float dt;
	float diffusion_coeff;
	float decay_coeff;
	struct Matrix U;
	struct Matrix U_tmp;
	struct Matrix U_laplacian;
	struct Vector diff_kernel;

	struct Renderer* renderer;
}; // struct Animation


extern int
animation_init(
	struct Animation* self,
	int screen_width,
	int screen_height
);


extern void
animation_destroy(
	struct Animation* self
);


extern void
animation_handle_mouse_event(
	struct Animation* self,
	float x,
	float y
);


extern void
animation_render(
	const struct Animation* self,
	SDL_Surface* surface
);


extern void
animation_update(
	struct Animation* self
);


#endif /* PESTACLE_ANIMATION_H */ 