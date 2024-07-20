#ifndef PESTACLE_RENDERER_H
#define PESTACLE_RENDERER_H

#include <SDL.h>
#include "matrix.h"


/*
 * Interface defining a way to render a matrix to a SDL buffer
 */


struct s_Renderer;
typedef struct s_Renderer Renderer;


typedef struct {
	const char* name;

	int (*setup)(
		Renderer*,
		int width,
		int height
	);

	void (*destroy)(
		Renderer*
	);

	void (*render)(
		Renderer*,
		const Matrix* src,
		SDL_Surface* dst
	);
} RendererDelegate;


struct s_Renderer {
	void* data;
	const RendererDelegate* delegate;
}; // struct s_Renderer


extern Renderer*
renderer_allocate();


extern int
renderer_setup(
	Renderer* self,
	int width,
	int height
);


extern void
renderer_destroy(
	Renderer* self
);


extern void
renderer_render(
	Renderer* self,
	const Matrix* src,
	SDL_Surface* dst
);


#endif /* PESTACLE_RENDERER_H */